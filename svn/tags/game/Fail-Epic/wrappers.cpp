#include "WPILib.h"
#include "wrappers.h"
#include "util.h"
#include "drive.h"
#include "oi.h"
#include "compile config.h"

#include <taskLib.h>
#include <usrLib.h>


extern bool rampEle; // fuck accessors

// entry point for thread to update motor ramping
int ramp_motors(...)
{
	Timer t;
	t.Start();
	for(;;) {
		Wait(0.1);
		hal->seekToGoals(t.Get());
		t.Reset();
	}
}

Wrapper::Wrapper() :
	// more than we'll ever need
	motors(100),
	joysticks(100)
{
	gyroObject = new Gyro(g1Channel);
	motors[M_RIGHT]    = new Jaguar(mRightChannel);
	motors[M_LEFT]     = new Jaguar(mLeftChannel);
	motors[M_SHOOTER]  = new Victor(mShooterChannel);
#ifndef DUMPER
	motors[M_TURRET]   = new Jaguar(mTurretChannel);
#endif
	motors[M_INTAKE]   = new Victor(mIntakeChannel);
	motors[M_ELEVATOR1] = new Victor(mElevatorChannel1);
	motors[M_ELEVATOR2] = new Victor(mElevatorChannel2);
	
	turretPotentiometer = new AnalogChannel(pTurretChannel);
	
	/*
	turretPIDController = new PIDController(turretP, turretI, turretD); // 50ms update time unless specified
	turretPIDSource = new PotentiometerPIDSource(turretPotentiometer);
	turretPIDOutput = new PotentiometerPIDOutput(motors[M_TURRET]);
	
	turretPIDController->SetInput(turretPIDSource, -1.f, 1.f);
	turretPIDController->SetOutput(turretPIDOutput, -.2, .2); // should be -1 to 1, but testing, so slower
	*/
	
	joysticks[J_1] = new Joystick(j1Port);
	joysticks[J_2] = new Joystick(j2Port);
	
	gyroObject->Reset();
	
	wrapperMutex = semMCreate(SEM_Q_PRIORITY | SEM_INVERSION_SAFE | SEM_DELETE_SAFE);
	
	transmissionRightEncoder = new VelEncoder(transmissionDiameter, transmissionClicksPerRotation,
			                                  eTransmissionRightA,eTransmissionRightB,true);
	transmissionLeftEncoder = new VelEncoder(transmissionDiameter, transmissionClicksPerRotation,
				                                  eTransmissionLeftA,eTransmissionLeftB,true);
	omniRightEncoder = new VelEncoder(omniDiameter, omniClicksPerRotation, eOmniRightA,eOmniRightB);
	omniLeftEncoder = new VelEncoder(omniDiameter, omniClicksPerRotation, eOmniLeftA, eOmniLeftB);
	
	accelRight = new AccelEncoder(transmissionRightEncoder);
	accelLeft = new AccelEncoder(transmissionLeftEncoder);
	
#ifdef WOBBLE
	wobbleSwitch = new DigitalInput(wobbleChannel);
#endif
	
	goalIntake = goalElevator = 0.0;
}
Wrapper::~Wrapper()
{
	// the vectors are oversized
	// don't delete them, as that would end up freeing unmalloced memory
	/*for (unsigned char i = 0; i < motors.size(); i++)
		delete motors[i];
	for (unsigned char i = 0; i < joysticks.size(); i++)
		delete joysticks[i];*/
	delete gyroObject;
	
	delete omniLeftEncoder;
	delete omniRightEncoder;
	delete transmissionLeftEncoder;
	delete transmissionRightEncoder;
	
	delete turretPotentiometer;
	
	semDelete(wrapperMutex);
}

float Wrapper::getManualShooterSpeed()
{
	//float speedSwitchValue = DriverStation::GetInstance()->GetAnalogIn(oi->iShooterSpeedChannel);
	float dial = DriverStation::GetInstance()->GetAnalogIn(1);
	/*if(speedSwitchValue < oi->midLow) {
		return highShooterSpeed;
	}
	else if(speedSwitchValue < oi->midHigh) {
		return midShooterSpeed;
	}
	else {
		//return lowShooterSpeed;
		return midShooterSpeed;
	}*/
	
	float normalized = (dial - 200.0) / 600.0;
	return -(normalized * 0.7 + 0.2);
	
	//return (hal->getJoystickThrottle(J_1) + 1.0) / 2.0;
}

///////////////////////////////////////////////////////
// Joystick Functions

Coords2Df Wrapper::getJoystickPosition(JOYSTICK j)
{
	CRITICAL_REGION(wrapperMutex)
		return Coords2Df(joysticks[j]->GetX(), joysticks[j]->GetY());
	END_REGION
}

float Wrapper::getJoystickX(JOYSTICK j)
{ 
	CRITICAL_REGION(wrapperMutex)
	return getJoystickPosition(j).x;
	END_REGION
}

float Wrapper::getJoystickY(JOYSTICK j)
{
	CRITICAL_REGION(wrapperMutex)
	return getJoystickPosition(j).y;
	END_REGION
}

float Wrapper::getJoystickThrottle(JOYSTICK j)
{
	CRITICAL_REGION(wrapperMutex)
		return joysticks[j]->GetThrottle();
	END_REGION
}

bool Wrapper::getJoystickTrigger(JOYSTICK j)
{
	CRITICAL_REGION(wrapperMutex)
		return joysticks[j]->GetTrigger();
	END_REGION
}

bool Wrapper::getJoystickTopButton(JOYSTICK j)
{
	CRITICAL_REGION(wrapperMutex)
		return joysticks[j]->GetTop();
	END_REGION
}

bool Wrapper::getJoystickButton(JOYSTICK j, unsigned char buttonId)
{
	CRITICAL_REGION(wrapperMutex)
	return joysticks[j]->GetRawButton(buttonId);
	END_REGION
}

///////////////////////////////////////////////////////
// Motor Functions

void  Wrapper::setMotorSpeed(MOTOR m, float power)
{
	CRITICAL_REGION(wrapperMutex)
		motors[m]->Set(power);
	END_REGION
}

float Wrapper::getLeftAccel()
{
	return accelRight->getAccel();
}
float Wrapper::getRightAccel()
{
	return accelLeft->getAccel();
}

void Wrapper::setMotorLeft(float power)
//{ leftJag.Set(power); }
{ setMotorSpeed(M_LEFT,power); }

void Wrapper::setMotorRight(float power)
{
	//cout << "setting right to " << power;
	//Jaguar *m = (Jaguar *)motors[M_RIGHT];
	//m->Set(power);
	//rightJag.Set(power);
	//setMotorSpeed(M_RIGHT, power);
	//cout << " now right = " << motors[M_RIGHT]->Get() << endl;
	setMotorSpeed(M_RIGHT,power);
}

float Wrapper::getMotorSpeed(MOTOR m)
{
	CRITICAL_REGION(wrapperMutex)
		return motors[m]->Get();
	END_REGION
}

float Wrapper::getMotorLeft()
{ return getMotorSpeed(M_LEFT); }
//{ return leftJag.Get(); }

float Wrapper::getMotorRight()
{ return getMotorSpeed(M_RIGHT); }
//{ return rightJag.Get(); }

float Wrapper::getWheelVelocity(SIDE side, WHEEL_TYPE wheelType)
{
	CRITICAL_REGION(wrapperMutex)
	if (side == SIDE_RIGHT) {
		if (wheelType == OMNI) {
			return omniRightEncoder->getVel();
		} else { // wheelType = TRANSMISSION
			return transmissionRightEncoder->getVel();
		}
	} else { // side = SIDE_LEFT
		if (wheelType == OMNI) {
			return omniLeftEncoder->getVel();
		} else { // wheelType == TRANSMISSION
			return transmissionLeftEncoder->getVel();
		}
	}
	END_REGION
}

float Wrapper::getWheelDistance(SIDE side, WHEEL_TYPE wheelType)
{
	CRITICAL_REGION(wrapperMutex)
	if (side == SIDE_RIGHT) {
		if (wheelType == OMNI) {
			return omniRightEncoder->getDistance();
		} else { // wheelType = TRANSMISSION
			return transmissionRightEncoder->getDistance();
		}
	} else { // side = SIDE_LEFT
		if (wheelType == OMNI) {
			return omniLeftEncoder->getDistance();
		} else { // wheelType == TRANSMISSION
			return transmissionLeftEncoder->getDistance();
		}
	}
	END_REGION
}

#if defined(EPIC) || defined(FAIL)
void Wrapper::suckerSuck()
{ setMotorSpeed(M_INTAKE, 1.0); }

void Wrapper::suckerBlow()
{ setMotorSpeed(M_INTAKE, -1.0); }

void Wrapper::suckerStop()
{ setMotorSpeed(M_INTAKE, 0.0); }

void Wrapper::setSuckerSpeed(float f)
{ setMotorSpeed(M_INTAKE, f); }

void Wrapper::setElevatorSpeed(float f)
{
	setMotorSpeed(M_ELEVATOR1, f);
	setMotorSpeed(M_ELEVATOR2, -f);
}

void Wrapper::elevatorUp()
{
	setElevatorGoal(1.0);
}

void Wrapper::elevatorDown()
{
	setElevatorGoal(-1.0);
}

void Wrapper::elevatorStop()
{
	setElevatorGoal(0.0);
}

float Wrapper::angleToPotentiometerVolts(float angle) // 0 = center, -90 = right, 90 = left
{
	if(angle > 1 || angle < 0) {
		printf("angle not in angleToPotentiometerVoltVal range: %f\n", angle);
		return 2.7; // center volt value
	}
	return (angle + 90)/pDegreesPerVolt + pMinVolts;
}

float Wrapper::getTurretAngle() // in degrees: 0 = center, -90 = right, 90 = left
{
#ifndef DUMPER
	return (turretPotentiometer->GetVoltage() - pMinVolts) * pDegreesPerVolt - 90.0;
#else
	return 0;
#endif
}

void Wrapper::startShooting(float shootSpeed)
//I think it should do something other than simply set the shooter to the given value - we already have a function that does that.
//											-Benjamin
{
	setMotorSpeed(M_SHOOTER, shootSpeed); 
	elevatorUp();
}

void Wrapper::stopShooting()
{
	setMotorSpeed(M_SHOOTER, 0.0);
	elevatorStop();
	// TODO: get ready to fire?
}

/* void Wrapper::rotateTurret(int degrees) {
	
} */


#endif

void Wrapper::seekToGoals(float tdelta)
{
	const float maxDeltaPerSecondIntake = 2.0;
	const float maxDeltaPerSecondElevator = 5.0;
	
	float currentElevator = getMotorSpeed(M_ELEVATOR1);
	float newElevator = getNewValueFromGoal(currentElevator, goalElevator, tdelta * maxDeltaPerSecondElevator);
	if (rampEle)
		setElevatorSpeed(newElevator);
	//cout << "current = " << currentElevator << " new = " << newElevator << " goal = " << goalElevator << endl;
	
	setSuckerSpeed(getNewValueFromGoal(getMotorSpeed(M_INTAKE),
				                       goalIntake,
				                       tdelta * maxDeltaPerSecondIntake));
}

float Wrapper::getNewValueFromGoal(float current,float goal,float delta)
{
	// within range
	if (abs(current - goal) <= delta)
		return goal;
	
	// not high enough
	if (current < goal) return current + delta;
	// too high
	if (current > goal) return current - delta;
	
	// should never occur (UNLESS DELTA < 0)
	// (which it is a small percentage of the time)
	cout << "halp! shouldn't ever occur! delta = " << delta << endl;
	return goal;
}

void Wrapper::setElevatorGoal(float goal)
{
	goalElevator = driver->clamp(goal);
}
void Wrapper::setIntakeGoal(float goal)
{
	goalIntake = driver->clamp(goal);
}

#ifdef WOBBLE
///////////////////////////////////////////////////////
// Wobble Switch Functions
bool Wrapper::getWobbleState()
{	return !(bool)(wobbleSwitch->Get());	}
void Wrapper::updateWobbleState()
{	oldWobbleState = bool(getWobbleState());	}
bool Wrapper::getOldState()
{	return bool(oldWobbleState);	}
bool Wrapper::isNewWobbleTrip()
{
	bool returnValue = false;
	if (getWobbleState() && !oldWobbleState)
		returnValue = true;
	oldWobbleState = getWobbleState();
	return returnValue;
}
#endif


///////////////////////////////////////////////////////
// Other Functions
float Wrapper::getByPWM(int pwm)
{
	CRITICAL_REGION(wrapperMutex)
		return (PWM(static_cast<unsigned char>(pwm)).GetRaw()-127)/255.0;
	END_REGION
}

// Why would anyone want this? As far as I'm aware, every other function
// we, and WPILIB, have uses floats, not unsigned 1 byte integers.
UINT8 Wrapper::getByPWMRaw(int pwm)
{
	CRITICAL_REGION(wrapperMutex)
		return PWM(static_cast<unsigned char>(pwm)).GetRaw();
	END_REGION
}

float Wrapper::getAngle()
{
	CRITICAL_REGION(wrapperMutex)
		return gyroObject->GetAngle();
	END_REGION
}
