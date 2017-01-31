#include "control.h"
#include "driver.h"
#include "hal.h"
#include "hanger.h"
#include <math.h>
#include "WPILib.h"
#include "script.h"
#include "locator.h"
#include "camera.h"
#include "suckControl.h"

Control* const control = new Control();

#define BETWEEN(START, END, VAL)\
	((VAL) >= (START) && (VAL) <= (END))

float Control::deadzone(float x, float radius)
{
	if (fabs(x) < radius) return 0.0;
	return x;
}


void Control::runOperator()
{
//#define KEEGAN_DRIVE
#ifndef KEEGAN_DRIVE
	float x = hal->lStick.GetX(), y = -hal->lStick.GetY(), r = hal->rStick.GetX();
#else
	float x = hal->lStick.GetX(), y = -hal->lStick.GetY(), r = hal->lStick.GetRawAxis(4);
#endif
	driver->drive(deadzone(x),deadzone(y),deadzone(r));

#if 0
	static unsigned int c = 0;

	if (c > 300)
	{
		for (unsigned int i = 1; i <= 6; ++i)
			printf("RawAxis[%u] = %f\n", i, hal->hStick.GetRawAxis(i));

		c = 0;
	}

	++c;
#endif

#if 0
	//to test servo
	if (hal->hStick.GetRawButton(6) &&
		hal->hStick.GetRawButton(11))
	{
		hanger->extendBar();
	}
	
	// HANGER DEPLOY
	/*
	static bool lastHStickTopBtn = false;
	bool newHStickTopBtn = false; //hal->hStick.GetTrigger();
	if (newHStickTopBtn && !lastHStickTopBtn)
		hanger->extendBar();
	lastHStickTopBtn = newHStickTopBtn;
	*/
	
	// HANGER WINCH
	hanger->retractWinch(0.f);
	if (hanger->hasBarExtended())
	{
		hanger->retractWinch(deadzone(-hal->hStick.GetY()));
		
		if (hal->hStick.GetRawButton(5))
			hal->setArm(false);
	}
	
	// HANGER BREAK
#ifdef PYRO
	if (hanger->hasBarExtended())
	{
		if (hal->hStick.GetRawButton(8))
			hal->setHangerBreak(true);
		else if (hal->hStick.GetRawButton(9))
			hal->setHangerBreak(false);
	}
#endif
#endif
	// BALL SUCKER
	sucker->stop();
	if (hal->lStick.GetTrigger())
		sucker->blow();
	if (hal->rStick.GetTrigger())
		sucker->suck();
	
	// KICK WINCH
	/*hal->setWinchShootPower(0);
	if (hal->hStick.GetRawButton(3))
		hal->setWinchShootPower(1);
	if (hal->hStick.GetRawButton(2))
		hal->setWinchShootPower(-1);*/
#ifdef PYRO
	float bottom = 6;
	float top = 12.5; //(hal->lStick.GetThrottle() + 1.0) / 2.0 * 5.0 + 10.0;
	float range = top - bottom;
	
#if 0
	static int tmr_ = 0;
	if (tmr_++ >= 5) { tmr_ = 0; printf("%f\n",top); }
#endif
	
#else
	const float bottom = 6.0;
	const float range = 7.0;
#endif
#if 1
	script->setKickWinchThreshold(((-hal->hStick.GetThrottle()) + 1.f) / 2.f * range + bottom);
	/*if (hal->hStick.GetTop() || hal->lStick.GetTop())
		script->runKickScript();*/

	// KICKING + AUTOCOCK
	static Timer autocockTimer;
	static bool hasInitedAutocock = false;
	if (!hasInitedAutocock) {
		hasInitedAutocock = true;
		autocockTimer.Start();
	}
	if (hal->rStick.GetTop() || hal->hStick.GetTrigger()) {
		hal->setKickerLatch(true);
		sucker->blow();
		autocockTimer.Reset();
	} else {
		if (autocockTimer.Get() > 0.3)
			script->runKickScript();
	}
	if (hal->hStick.GetRawButton(6)) script->abortScript();
#endif
	
	static bool lastB6 = false, lastB7 = false, lastB11 = false, lastB10 = false;
	bool newB6 = hal->lStick.GetRawButton(6),
	     newB7 = hal->lStick.GetRawButton(7),
	     newB10 = hal->lStick.GetRawButton(10),
	     newB11 = hal->lStick.GetRawButton(11);
	
	if (newB6 && newB7 && newB10 && newB11 &&
		(!lastB6 || !lastB7 || !lastB10 || !lastB11))
	{
		setDiagnosticsModeEnabled(true);
	}
	lastB6 = newB6;
	lastB7 = newB7;
	lastB10 = newB10;
	lastB11 = newB11;
}

void Control::runDiagnostics()
{
	static bool lastB6 = false, lastB7 = false, lastB11 = false, lastB10 = false;
	bool newB6 = hal->rStick.GetRawButton(6),
	     newB7 = hal->rStick.GetRawButton(7),
	     newB10 = hal->rStick.GetRawButton(10),
	     newB11 = hal->rStick.GetRawButton(11);
	
	if (newB6 && newB7 && newB10 && newB11 &&
		(!lastB6 || !lastB7 || !lastB10 || !lastB11))
	{
		setDiagnosticsModeEnabled(false);
	}
	lastB6 = newB6;
	lastB7 = newB7;
	lastB10 = newB10;
	lastB11 = newB11;
}

void Control::runDisabled()
{
	//static const char* automodeStrings[] = { "mode0", "mode1", "mode2" };

	float lThrottle = (hal->lStick.GetThrottle() + 1.f)/2.f;
	
	if (BETWEEN(0.0f, 0.2f, lThrottle))
		mode = mode0;
	else if (BETWEEN(0.4f, 0.6f, lThrottle))
		mode = mode1;
	else if (BETWEEN(0.8f, 1.0f, lThrottle))
		mode = mode2;
	
	static int tmr = 0;
	if (tmr++ > 10000)
	{
		tmr = 0;
		//printf("mode = %s\n", automodeStrings[mode]);
	}
}

void Control::runAutonomous()
{	
	// i feel insulted. really, first? really?
	// why?
	
	
	//printf("%i",(int) mode);
	Wait(0.01);
	static Timer tmr;
	static bool inited = false;
	if (!inited) {
		inited = true;
		tmr.Start();
	}
	
	//if (mode == mode0)
	//	runScoreAuto();
	//else if (mode == mode1)
		runMidAuto();
	//else if (mode == mode2)
	//	runDefAuto();
}

void Control::runMidAuto()
{
	enum autostate { initialize, waitForCock, goForward, kickFollowthrough, waitForCock2, goForward2, kickFollowthrough2, done };
	static autostate state = initialize;
	const float speed = 0.3;
	const float distanceToFirstBall = 24;
	const float distanceToSecondBall = 95 + 4 + 7;
	const float followthroughDistance = 6;
	static Timer tmr;
	switch (state) {
	case initialize:
		tmr.Start();
		state = waitForCock;
		break;
	case waitForCock:
		script->runKickScript();
		script->setKickWinchThreshold(12);
		driver->drive(0,0,0);
		if (tmr.Get() > 4.5) state = goForward;
		break;
	case goForward:
		driver->drive(0.0,speed,0.0);
		sucker->suck();
		script->runKickScript();
		script->setKickWinchThreshold(12);
		if (locator->getX() > distanceToFirstBall) {
			//printf("Got to first ball");
			hal->setKickerLatch(true);
			sucker->blow();
			state = kickFollowthrough;
		}
		break;
	case kickFollowthrough:
		driver->drive(0.0,speed * 0.7,0.0);
		sucker->blow();
		hal->setKickerLatch(true);
		if (locator->getX() > distanceToFirstBall + followthroughDistance) {
			//pritnf("followed through");
			state = waitForCock2;
			tmr.Reset();
		}
		break;
	case waitForCock2:
		driver->drive(0.0,0.0,0.0);
		sucker->stop();
		script->runKickScript();
		script->setKickWinchThreshold(12);
		if (tmr.Get() > 3.5) {
			state = goForward2;
			//printf("done cocking");
		}
		break;
	case goForward2:
	{
		/*float dy = locator->getY();
		float ky = -0.1;*/
		float rspeed = -0.05 / 3.0; //speed / 4;
		driver->drive(0,speed,rspeed/*dy * ky*/);
		sucker->suck();
		script->runKickScript();
		script->setKickWinchThreshold(12);
		if (locator->getX() > distanceToSecondBall) {
			hal->setKickerLatch(true);
			sucker->blow();
			state = kickFollowthrough2;
		}
	}
		break;
	case kickFollowthrough2:
		driver->drive(0.0,speed,0.0);
		sucker->blow();
		hal->setKickerLatch(true);
		if (locator->getX() > distanceToSecondBall + followthroughDistance) {
			state = done;
		}
		break;
	case done:
		script->abortScript();
		hal->setKickerLatch(true);
		driver->drive(0.0,0.0,0.0);
		sucker->stop();
		break;
	}
}

void Control::runDefAuto()
{
	// Go forward while running intake.
	// If voltage required to reach current drops below some threshold, kick
	const float speed = 0.6;
	const float voltageThreshold = -0.3;
	
	sucker->suck();
	driver->drive(0.0,speed,0.0);
	
	if (sucker->getVoltage() > voltageThreshold)
		hal->setKickerLatch(true);
	
	script->setKickWinchThreshold(10.5);
	script->runKickScript();
}

void Control::runScoreAuto()
{
	enum autostate { goForward, kickBall, moveAway, doneAuto };
	static autostate state = goForward;
	const float distanceToBall = 95;
	const float followthroughDistance = 6; //12;
	const float minX = 0; //18;
	const float maxY = 3 * 12; //12 * (27 - 5);
	const float speed = 1.0;
	const float rotSpeed = -0.06; // turn with a curve
	const float retreatDistance = 14;
	//const float moveAwayStrafe = 0.3;
	switch (state) {
	case goForward:
		// cock kicker
		script->runKickScript();
		script->setKickWinchThreshold(9);
		driver->drive(0.0,speed * 0.6,rotSpeed * 0.6);
		sucker->suck();
		if (locator->getX() > distanceToBall) {
			state = kickBall;
			printf("Done going forward\n");
		}
		break;
	case kickBall:
		driver->drive(0.0,speed * 0.6,0.0);
		hal->setKickerLatch(true);
		sucker->suck();
		if (locator->getX() > distanceToBall + followthroughDistance) {
			state = moveAway;
			printf("done following through\n");
		}
		break;
	case moveAway:
		sucker->stop();
		driver->drive(0,-speed,-rotSpeed * 3); // or -1.0, which is it? i think it's positive
		if (locator->getX() < retreatDistance) {
			state = doneAuto;
			printf("done moving away\n");
		}
		break;
	case doneAuto:
		driver->drive(0.0,0.0,0.0);
		break;
	}
}

void Control::initAutonomous()
{ locator->reset(); }

