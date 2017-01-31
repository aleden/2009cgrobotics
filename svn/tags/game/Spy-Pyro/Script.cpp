#include "script.h"
#include "HAL.h"
#include "driver.h"
#include "hanger.h"
#include "WPILib.h"
#include <math.h>

Script* const script = new Script();

 void Script::checkScriptButtons(void){
	 // this should have been in control
/*	 bool inBumpMode = hal->getOverBumpButton();
	 bool inWinchMode = hal->getWinchHangButton();
	 bool inKickMode = hal->getKickerButton();
	 bool aborted = hal->getScriptAbortButton();
	 if (inBumpMode){
		 currentScript = bumpID;
	 }	
	 else if (inWinchMode){
		 currentScript = winchID;
	 }	
	 else if (inKickMode){
		 currentScript = kickerID;
		 printf("In kick mode");
	 }
	 if (aborted){
		 currentScript = none;
		 setScriptPosition(pos0);
		 printf("ABORT");
	 }*/
	 bool inKickMode = hal->getKickerButton();
	 bool aborted = hal->getScriptAbortButton();
	 if (inKickMode){
			 currentScript = kickerID;
			 //printf("In kick mode");
		 }
	 if (aborted){
		 abortScript();
		 //printf("ABORT");
	 }
 }

void Script::setKickWinchThreshold(float threshold)
{
	kickWinchThreshold = threshold;
}

void Script::runKickScript(void){  // for auto mode
	kickerScriptRunning = true;
	currentScript = kickerID;
}

void Script::abortScript(void){  // for auto mode
	currentScript = none;
	setScriptPosition(pos0);
	hal->setWinchShootPower(0.0);
}

scriptID Script::getCurrentScript(void){
	script->checkScriptButtons();
	return currentScript;
}

scriptPos Script::getScriptPosition(void){
	return scriptPosition;
}

void Script::setScriptPosition(scriptPos position){
	scriptPosition = position;
}

void Script::initAutonomous(void)
{
	currentScript = none;
}

void Script::initOperator(void)
{
	currentScript = none;
}

void Script::runAutonomous(void)
{
	//needs this to make autonomous scripts not dependent on joystick buttons
	if (kickerScriptRunning){
		currentScript = kickerID;
	}
	else{
		currentScript = none;
	}
	
	scriptID id = script->getCurrentScript();
	runScripts(id);
}

void Script::runOperator(void)
{
	scriptID id = script->getCurrentScript();
	runScripts(id);
}

// getCurrentScript should be called in operator mode only
// because it relies on joystick buttons.hal->setArm(true);
// Auto mode needs to rely on runKickScript method only.
void Script::runScripts(scriptID id)
{
	static Timer tmr, timer;
	const float slidebackTime = 0.6;
	static float savedThreshold;
	static const float windBackTime = 1.5;
	static const float cylinderForwardTime = .3;
	if (id == kickerID)
	{
		scriptPos currentPosition = script->getScriptPosition();
		//printf("running kick script\n");
		// Steps for kicking:
		// 1. Open latch
		// 2. Push kicker forward into latch
		// 3. Close latch
		// 4. Winch kicker back until desired current is reached
		// 5. Exit (actual kicking (the unlatching) is done by the user)
		
		switch(currentPosition)
		{
		case pos0:
			script->setScriptPosition(pos1);
			timer.Start();
			timer.Reset();
			//printf("STARTING KICK SCRIPT");
		case pos1:
			hal->setKickerLatch(true); // open up latch
			hal->setWinchShootPower(1); // push kicker back into latch
			//printf("MOVING KICKER TO LATCH");
			if (timer.Get() > windBackTime)
			{
				// kicker winch is out far enough
				hal->setKickerForwardValue(true);
				timer.Start();
				timer.Reset();
				script->setScriptPosition(pos1b);
			}
			
			// On failure: script->setScriptPosition(pos5);
			break;
			
		case pos1b:
			if (timer.Get() > cylinderForwardTime)
			{
				hal->setKickerLatch(false); // close latch
				hal->setKickerForwardValue(false); // close kicker cylinder
				script->setScriptPosition(pos2);
			}
			break;
			
		case pos2:
		{
			float power = hal->getShooterCurrent();
			const float stopRadius = 0.5;
			float delta = fabs(power - kickWinchThreshold);
			//printf("%f %f|",power,kickWinchThreshold);
			if (delta < stopRadius) {
				script->setScriptPosition(pos4);
				savedThreshold = kickWinchThreshold;
				//printf("DONE");
			} else if (power > kickWinchThreshold){
				// slide back a bit
				script->setScriptPosition(pos3);
				tmr.Start();
				tmr.Reset();
				hal->setWinchShootPower(1);
				//printf("SLIDE");
			} else {
				hal->setWinchShootPower(-1);
				//printf("UP");
			}
			if (hal->getKickerLatch()) {
				//printf("DONE COCK\n");
				script->setScriptPosition(pos0);
				currentScript = none;
				kickerScriptRunning = false;
				hal->setWinchShootPower(0);
			}
		}
			break;
		case pos3:
			hal->setWinchShootPower(1);
			if (tmr.Get() > slidebackTime) {
				script->setScriptPosition(pos2);
				//printf("TM");
			}
			if (hal->getKickerLatch()) {
				//printf("DONE COCK\n");
				script->setScriptPosition(pos0);
				currentScript = none;
				kickerScriptRunning = false;
				hal->setWinchShootPower(0);
			}
			break;
		case pos4: {
			hal->setWinchShootPower(0);
			const float stopRadius = 0.5;
			float delta = fabs(savedThreshold - kickWinchThreshold);
			if (delta > stopRadius)
				script->setScriptPosition(pos2); }
		if (hal->getKickerLatch()) {
			//printf("DONE COCK\n");
			script->setScriptPosition(pos0);
			currentScript = none;
			kickerScriptRunning = false;
			hal->setWinchShootPower(0);
		}
		break;
		case pos5: // latch failure, wind it back
			hal->setWinchShootPower(-1);
			//printf("F");
			if (hal->getWinchBreaker()) {
				
				script->setScriptPosition(pos0);
			}
			
			break;
		default:
			//printf("ERROR: INVALID POS FOR SCRIPT SHOOT");
			break;
		}
		
		return;
#if 0
		if (currentPosition == pos0){
			script->setScriptPosition(pos1);
		}
		else if (currentPosition == pos1){
			//hal->setKickerLatch(!hal->getNewKickerValue());
			//hal->setWinchShootPower(1);
			hal->setKickerLatch(false); // open up latch
			hal->setShooterCurrent(1); // Push forward
			script->setScriptPosition(pos2);
		}
		else if (currentPosition == pos2){
			if (hal->getShooterCurrent() > kickWinchThreshold){
				hal->setWinchShootPower(0);
				hal->setKickerLatch(!hal->getNewKickerValue());
				hal->setWinchShootPower(-1);
				script->setScriptPosition(pos3);
			}
		}
		else if (currentPosition == pos3){
			if (hal->getShooterCurrent() > kickWinchThreshold){
				hal->setWinchShootPower(0);
				script->setScriptPosition(pos0);
				currentScript = none;
			}
		}
#endif
	}
	
	else
	{
		currentScript = none;
	}
}
