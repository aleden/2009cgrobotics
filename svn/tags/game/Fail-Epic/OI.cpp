#include "OI.h"
#include "wrappers.h"
#include "drive.h"
#include "util.h"
#include "CatlinPID.h"
#include "Turret.h"
#include "Camera.h"
#include "AutoTarget.h"
#include "RobotActions.h"
#include "AutoTarget.h"

OIRoutines::OIRoutines()
{}

OIRoutines::~OIRoutines()
{}

void OIRoutines::operatorControlRoutine()
{	
	//////////////////////////////////////////////////////////////////////
	// Digital Buttons
	// D1 – Fire
	// Type: Button 
	// Job: Runs the elevator motor and runs the shooter motor for a set 
	// amount of time, will override any auto functions  
	// 
	// D2 – blow out balls 
	// Type: Button
	// Job: Runs the bottom motor backwards and runs the elevator down 
	//
	// D3 – manual elevator up
	// Type: button
	// Job: runs the elevator up, overrriding everything
	//
	// D4 – manual elevator down
	// Type: button
	// Job: runs the elevator down, overriding all other functions
	//////////////////////////////////////////////////////////////////////
	// Analog Buttons 
	// 
	// An1 – Turret Knob 
	// Type: Knob
	// Job: Aims the turret when in manual mode 
	// 
	// An2- Auto track and fire / auto track / manual 
	// Type: Three position Toggle
	// Job: Up is auto-track and fire; it has the turret scanning and the 
	// auto fire on. The middle position is just auto-track, this means 
	// that auto fire is off but the turret still scans. The Down position 
	// is completely manual enabling the manual aim knob. 
	// 
	// An3- Prepare to Fire / Off / Collect Balls
	// Type: Three position Toggle
	// Job: Up is prepare to fire, it has the elevator going up, until it 
	// is full and the intake motor is running forward? Down is collect 
	// balls. Middle turns off all ball handling motors.
	// 
	//////////////////////////////////////////////////////////////////////
	// Lights 
	// LED1 – Target Acquired Light 
	// Job: Lights up when the camera spots a target 
	// 
	// LED2 – Fire Light 
	// Job: Lights up when the camera thinks it would be a good time to fire
	/* static DigitalOutput *led1 = new DigitalOutput(1),
						 *led2 = new DigitalOutput(2); */
	// Beepers
	// D3 - beep
	// type: beeper!
	// job: beeps faster the closer target is, set to low in order to beep
	
	DriverStation* driver = DriverStation::GetInstance(); // will have the dig inputs attached to driver station
	//static int printTimer = 0;
	//printTimer++;
	//if(printTimer%50 == 0) {
		//printf("elevator: %f\n", driver->GetAnalogIn(sElevatorChannel));
		//printf("turret: %f\n", driver->GetAnalogIn(sTurretChannel));
		//printf("fire/off/collect: %f\n", driver->GetAnalogIn(sFireOffCollectChannel));
		//printf("autoTrack + fire/track/manual: %f\n", driver->GetAnalogIn(sAutoTrackChannel));
		//for(int i = 1; i <= 8; i++) printf("%d:%d  ", i, driver->GetDigitalIn(i) ? 1 : 0);
		//for(int i = 1; i <= 4; i++) printf("%d:%6.2f  ", i, driver->GetAnalogIn(i));
		//printf("\n");
	//} 
	// analog ins:
	float turretKnob = driver->GetAnalogIn(iTurretChannel); // clockwise is high
	float autoTrack = driver->GetAnalogIn(iAutoTrackChannel); // high is track and fire, mid is track, low is manual
	float prepareOffCollect = driver->GetAnalogIn(iPrepareOffCollectChannel); // high: prepare, mid: off, low: collect
		
	const float degreesPerPot = -90.0 / 340.0;
	const float centerPot = 520;
	
#ifndef DUMPER
	turret->setToAngle((turretKnob - centerPot) * degreesPerPot);
#endif
	// the following two lines are for manual, nonpid control
	//turret->enablePIDControl = false;
	//hal->setMotorSpeed(M_TURRET,hal->getJoystickX(J_2));
	
	//cout << "theta = " << hal->getTurretAngle() << endl;
	
	bool manualFiring = true;
	
	if(hal->getJoystickButton(J_1,3)) robotActions->slowIntake();
	else robotActions->stopSlowIntake();
#ifndef DUMPER
	if(autoTrack < midLow) {
		turret->enablePIDControl = true;
		//turret->enablePIDControl = true;
		
		autotargeting->disableAutoPan();
		autotargeting->disableAutoFire();
	}
	else if(autoTrack < midHigh) {
		turret->enablePIDControl = false;
		turret->reset();
		
		autotargeting->enableAutoPan();
		autotargeting->disableAutoFire();
	}
	else {
		turret->reset();
		turret->enablePIDControl = false;
		
		manualFiring = false;
		
		autotargeting->enableAutoPan();
		autotargeting->enableAutoFire();
	}
#endif
	bool shouldTurnOff = false;
	
	if(prepareOffCollect < midLow) {
		robotActions->suck();
	}
	else if(prepareOffCollect < midHigh) {
		shouldTurnOff = true;
	}
	else { // don't have to worry about it already being prepared, checked later
		robotActions->prepareToFire();
	}
	
	// digital ins:
	if (driver->GetDigitalIn(iFireChannel) == iFireTrue
			|| hal->getJoystickButton(J_1, 2))
	{
		//printf("fire pressed");
		float shootSpeed = hal->getManualShooterSpeed();
		//cout << shootSpeed << endl;
		robotActions->fire(shootSpeed); // TODO: find the distance, base speed on distance
	}
	else 
	{
#ifndef DUMPER
		if (!autotargeting->isAutoFiring) // make sure that the auto targeting isn't firing now
#endif
		if (autoTrack > 512) // flip is pressed
			robotActions->fire(hal->lowShooterSpeed);
		else
			robotActions->stopFiring();
	}
	
	if (driver->GetDigitalIn(iBlowChannel) == iBlowTrue)
	{
		//printf("blow pressed");
		robotActions->blow();
		shouldTurnOff = false;
	}
	else
	{
		robotActions->stopBlowing();
	}
	
	if (driver->GetDigitalIn(iElevatorUpChannel) == iElevatorUpTrue)
	{
		//printf("elevatorUp pressed");
		robotActions->elevatorUp();
		shouldTurnOff = false;
	}
	else if (driver->GetDigitalIn(iElevatorDownChannel) == iElevatorDownTrue)
	{
		//printf("elevatorDown pressed");
		robotActions->elevatorDown();
		shouldTurnOff = false;
	}
	else
	{
		robotActions->elevatorStop();
	}
	
	if (shouldTurnOff)
		robotActions->off();

#ifndef DUMPER
	// Digital Outputs:
	if (autotargeting->isTargetAquired) // if camera spotted target
	{
		driver->SetDigitalOut(oTargetLEDChannel, oTargetLEDTrue); 
	} else {
		driver->SetDigitalOut(oTargetLEDChannel, !oTargetLEDTrue); 
	}
	
	if (autotargeting->isReadyToFire) // if camera thinks it would be a good time to fire
	{
		driver->SetDigitalOut(oFireLEDChannel, oFireLEDTrue);
		driver->SetDigitalOut(oTargetBeeperChannel, oTargetBeeperTrue);
	} else {
		driver->SetDigitalOut(oFireLEDChannel, !oFireLEDTrue);
		driver->SetDigitalOut(oTargetBeeperChannel, !oTargetBeeperTrue);
	}
#endif
}

void OIRoutines::testTurretPID()
{	
	Wait(0.1);
#ifndef DUMPER
	if (hal->getJoystickButton(J_1, 11))
		turret->setToAngle(turret->getAngle() + 18);
	if (hal->getJoystickButton(J_1, 10))
		turret->setToAngle(turret->getAngle() - 18);
#endif
}

void OIRoutines::testMotors()
{
	static float elevatorSpeed = 0.f;
	static float intakerSpeed  = 0.f;
	static float increase = .002f;
	
	//autotargetting->enableAutoPan(); // awesome
	
	/////////////////////////////////////////////////////////////////////////////////
	// Shooter
	if (hal->getJoystickTrigger(J_1))
		hal->setMotorSpeed(M_SHOOTER,-1.0);
		//hal->startShooting(-1.0);
	else
		hal->setMotorSpeed(M_SHOOTER,0.0);
		//hal->stopShooting();
	
	/////////////////////////////////////////////////////////////////////////////////
	// Turret
	//hal->setMotorSpeed(M_TURRET, hal->getJoystickX(J_2)/2);
	//oi->testTurretPID();
	
	/////////////////////////////////////////////////////////////////////////////////
	// Elevator
	// does not work
	if (hal->getJoystickButton(J_1, 11))
	{
		//elevatorSpeed = 1.0;
		cout << "JB11" << endl;
		hal->setMotorSpeed(M_ELEVATOR1, 1.0);
	} else { hal->setMotorSpeed(M_ELEVATOR1, 0.0); }
	
	if (hal->getJoystickButton(J_1, 10))
	{
		cout << "JB10" << endl;
		hal->setMotorSpeed(M_ELEVATOR2, -1.0);
		//elevatorSpeed = -1.0;
	} else { hal->setMotorSpeed(M_ELEVATOR2, 0.0); }
	
	if (hal->getJoystickButton(J_1, 5))
	{
		cout << "JB5" << endl;
		//elevatorSpeed = 0.f;
	}
	//hal->setElevatorGoal(elevatorSpeed);
	DriverStation* driver = DriverStation::GetInstance();
	if (hal->getWobbleState()) {
		driver->SetDigitalOut(oTargetLEDChannel, oTargetLEDTrue); 
		driver->SetDigitalOut(oFireLEDChannel, oFireLEDTrue); 
	} else {
		driver->SetDigitalOut(oTargetLEDChannel, !oTargetLEDTrue); 
		driver->SetDigitalOut(oFireLEDChannel, !oFireLEDTrue);
	}
		
#ifdef WOBBLE
	/*if (hal->getJoystickButton(J_2, 10) || hal->getJoystickButton(J_2, 11))
		hal->setElevatorSpeed(deadzone(elevatorSpeed, 0.f, .3f));
	if (hal->newWobbleTrip())
		hal->elevatorStop();*/
#else
	hal->setElevatorGoal(elevatorSpeed);
#endif
	/////////////////////////////////////////////////////////////////////////////////
	// Intaker
	if (hal->getJoystickButton(J_1, 6) && (intakerSpeed + increase) <= 1.f)
	{
		intakerSpeed = 1.0;
		cout << "JB6" << endl;
	}
	if (hal->getJoystickButton(J_1, 7) && (intakerSpeed - increase) >= -1.f)
	{
		intakerSpeed = 0.0;
		cout << "JB7" << endl;
	}
	
	if (hal->getJoystickButton(J_1, 4))
	{
		intakerSpeed = 0.f;
		cout << "JB4" << endl;
	}
		
	hal->setIntakeGoal(deadzone(intakerSpeed, 0.f, .3f));
	/////////////////////////////////////////////////////////////////////////////////
	// Misc
	//if (hal->getJoystickButton(J_2, 3))
	//	cout << "Button3 is down.\n";
}




