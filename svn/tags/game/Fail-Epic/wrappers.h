#pragma once
#include "Coords2D.h"
#include "compile config.h"
#include "VelEncoder.h"
#include "AccelEncoder.h"
#include <vector>
//#include "encoderpid.h"

int ramp_motors(...);

enum MOTOR
{
	M_RIGHT, M_LEFT, M_SHOOTER,
#ifndef DUMPER
	M_TURRET,
#endif
	M_INTAKE,
	M_ELEVATOR1, M_ELEVATOR2
};
enum SIDE
{ SIDE_RIGHT, SIDE_LEFT };
enum WHEEL_TYPE
{ OMNI, TRANSMISSION };
enum ENCODER_DIRECTION
{ ED_FORWARD, ED_REVERSE };
enum JOYSTICK
{ J_1, J_2 };


class Wrapper
{
public:
	// Prefixes explained:
	// m = motor
	// p = potentiometer
	// g = gyro
	// j = joystick
	// e = encoder
	
	///////////////////////////////////////////////////////
	// Motors
	static const unsigned char mRightChannel     = 2;
	static const unsigned char mLeftChannel      = 1; // changed from 3
	static const unsigned char mShooterChannel   = 7;
	static const unsigned char mTurretChannel    = 3;
	static const unsigned char mElevatorChannel1 = 4;
	static const unsigned char mElevatorChannel2 = 5;
	static const unsigned char mIntakeChannel    = 6;
#ifndef DUMPER
	// Speed values for manual speed control
	static const float lowShooterSpeed			= .6;
	static const float midShooterSpeed			= .8;
	static const float highShooterSpeed			= 1.0;
#else
	static const float lowShooterSpeed			= 1.0;
	static const float midShooterSpeed			= -0.3;
	static const float highShooterSpeed			= -0.5;
	static const float maxShooterSpeed          = -1.0;
#endif
	
	///////////////////////////////////////////////////////
	// Gyros
	static const unsigned char g1Channel = 2;
	
	///////////////////////////////////////////////////////
	// Joysticks
	static const unsigned char j1Port = 1;
	static const unsigned char j2Port = 2;
	
	///////////////////////////////////////////////////////
	// Encoders
	static const unsigned char eTransmissionLeftA  = 1;
	static const unsigned char eTransmissionLeftB  = 2;
	static const unsigned char eTransmissionRightA = 3;
	static const unsigned char eTransmissionRightB = 4;
	static const unsigned char eOmniLeftA          = 9;
	static const unsigned char eOmniLeftB          = 8;
	static const unsigned char eOmniRightA         = 6;
	static const unsigned char eOmniRightB         = 5;
	
	///////////////////////////////////////////////////////
	// turret potentiometer and PID
	static const unsigned char pTurretChannel = 7; // fake
	static const float	turretP	              = 1.f; // best value = ?
	static const float	turretI	              = 0.f; // best value = ?
	static const float	turretD	              = 0.f; // best value = ?
	static const float	pMaxVolts			  = 5.275f; // apporximate, assuming linear scale and 2.7V = center
	static const float	pMinVolts			  = .125f; // 90d right
	static const float	pDegreesPerVolt		  = 34.9515f; //180.f/(pMaxVolts - pMinVolts);
	
#ifdef WOBBLE
	///////////////////////////////////////////////////////
	// Wobble Switch
	static const unsigned char wobbleChannel = 10;
#endif
	
	///////////////////////////////////////////////////////
	// Wheel and encoder properties
	static const float omniDiameter = 2.75;
	static const float transmissionDiameter = 6.0;
	static const int omniClicksPerRotation = 90;
	static const int transmissionClicksPerRotation = 250;
	
private:
	// Motors and joysticks are a direct address table
	// for MOTOR and JOYSTICK enums to the actual
	// corrosponding motors and joysticks.
	std::vector<SpeedController*> motors;
	std::vector<Joystick*> joysticks;
	
	Gyro* gyroObject;
	
	VelEncoder *transmissionRightEncoder,
	           *transmissionLeftEncoder,
	           *omniRightEncoder,
	           *omniLeftEncoder;
	
	AccelEncoder *accelRight, *accelLeft;
	
	AnalogChannel* turretPotentiometer;
	
	SEM_ID wrapperMutex;
	
	// goal seeking
	float goalIntake, goalElevator;
	float getNewValueFromGoal(float current,float goal,float delta);

#ifdef WOBBLE
	// Wobble Switch
	DigitalInput* wobbleSwitch;
	bool oldWobbleState;
#endif
	
public:
	Wrapper();
	~Wrapper();
	
	///////////////////////////////////////////////////////
	// Joystick Functions
	Coords2Df getJoystickPosition(JOYSTICK j);
	float getJoystickX(JOYSTICK j);
	float getJoystickY(JOYSTICK j);
	float getJoystickThrottle(JOYSTICK j);
	bool getJoystickTrigger(JOYSTICK j);
	bool getJoystickTopButton(JOYSTICK j);
	bool getJoystickButton(JOYSTICK j, unsigned char buttonId);
	
	///////////////////////////////////////////////////////
	// Motor Functions
	void setMotorSpeed(MOTOR m, float power);
	void setMotorLeft(float power);
	void setMotorRight(float power);
	
	float getMotorSpeed(MOTOR m);
	float getMotorLeft();
	float getMotorRight();
	
	float getLeftAccel();
	float getRightAccel();
	
	void suckerSuck();
	void suckerBlow();
	void suckerStop();
	void setSuckerSpeed(float f);
	
	void elevatorUp();
	void elevatorDown();
	void elevatorStop();
	void setElevatorSpeed(float f);
	
	void startShooting(float shootSpeed);
	void stopShooting();
	
	void turretToAngle(float degrees);
	// void rotateTurret(float degrees);
	float getTurretAngle();
	
	void seekToGoals(float tdelta);
	void setElevatorGoal(float goal);
	void setIntakeGoal(float goal);
	
	float getManualShooterSpeed();
	
#ifdef WOBBLE
	///////////////////////////////////////////////////////
	// Wobble Switch Functions
	bool getWobbleState();
	bool isNewWobbleTrip();
	void updateWobbleState();
	bool getOldState();
#endif
	
private:
	float potentiometerVoltToAngle(float volts);
	float angleToPotentiometerVolts(float angle);

public:	
	///////////////////////////////////////////////////////
	// Misc functions
	float getWheelVelocity(SIDE side, WHEEL_TYPE wheelType);
	float getWheelDistance(SIDE side, WHEEL_TYPE wheelType);
	// Get the angle of the Robot relative to starting angle.
	float getAngle();
	float getByPWM(int pwm);
	UINT8 getByPWMRaw(int pwm);
};

// hal is short for Hardware Abstraction Layer.
// shorter way of accessing wrapper then using
// "wrapper".It actually exists in MyRobot.cpp.
// This is initialized in the entry point of 
// our program, which is RobotDemo::RobotDemo. 
extern Wrapper* hal;
