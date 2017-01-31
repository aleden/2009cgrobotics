#pragma once

class OIRoutines
{
private:
public:
	OIRoutines();
	~OIRoutines();
	
	// TODO: tune intake and elevator mtr speeds
	// The test motors function is used for testing
	// the motors with the second joystick. The
	// buttons 6 and 7 increase and decrease the
	// speed of the intaker, respectively. The
	// buttons 11 and 10 increase and decrease the
	// elevator speed, respectively. The trigger
	// runs the shooter's motor, and the throttle
	// controls the speed of the shooter when the
	// shooter's motor is run.
	void testMotors();
	
	void testTurretPID();
	
	void testActionScripts();
	
	void operatorControlRoutine();
	
	// OI:
	// inputs:
		// digital:
	static const int iFireChannel				= 1; // works
	static const bool iFireTrue					= false; // works
	static const int iBlowChannel				= 2; // works
	static const bool iBlowTrue					= false; // works
	static const int iElevatorUpChannel			= 3; // works
	static const bool iElevatorUpTrue			= false; // works
	static const int iElevatorDownChannel		= 4; // works
	static const bool iElevatorDownTrue			= false; // works
	
		// analog: float from 1-1023
	static const int deadZone					= 100; // not final, and not applied to turret and targetDistance
	static const int iTargetDistanceChannel		= 1; // works? might not even use
	// which direction is farther?
	static const int iTurretChannel				= 1; // works
	// which direction is clockwise? high (1023),
	static const int iPrepareOffCollectChannel	= 3; // works
	// up is 1000, mid is 470, down is 0 (down is prepare, mid is off, up is collect), deazone of 50
	static const int iAutoTrackChannel			= 2; // works
	// up is high, down is low, mid is 530, deadzone of 50
	static const int iShooterSpeedChannel		= 4;
	// up is low, down is high, mid is 530, deadzone of 50
	
	// btwn these is mid, lower than midLow is low, higher than midHigh is high
	static const float midHigh					= 682.7;
	static const float midLow					= 341.3;
	
	// outputs:
		// LEDs:
	static const int oTargetLEDChannel			= 1; // works
	static const bool oTargetLEDTrue			= false; // works
	static const int oFireLEDChannel			= 2; // works
	static const bool oFireLEDTrue				= false; // works
		// Beepers:
	static const int oTargetBeeperChannel		= 3; // works
	static const bool oTargetBeeperTrue			= false; // works
};

extern OIRoutines* oi;

