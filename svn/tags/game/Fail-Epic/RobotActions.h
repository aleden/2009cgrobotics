#pragma once
#include "WPILib.h"
#include "compile config.h"

enum BALL_MODE { BALL_COLLECT, BALL_OFF, BALL_PREPARE, BALL_READY_TO_FIRE };

class RobotActions
{
private:
	bool preFiring, firing, outaking, manualElevatorUp, manualElevatorDown, emptyCellCollecting;
	BALL_MODE mode;
	SEM_ID mutex;
	float fireSpeed;
	// static const float elevator
	
	bool enabled;
	
	Timer preFireTimer;
	
public:
	RobotActions();
	~RobotActions();
	
	void prepareToFire();
	
	void fire(float fireSpeed); // calls wrapper funct
	void stopFiring();
	
	void suck();
	void slowIntake(); // used for empty cells
	void stopSlowIntake();
	
	void blow();
	void stopBlowing();
	
	void elevatorUp();
	void elevatorDown();
	void elevatorStop();
	void off(); // sets all conveyor mtrs off
	
	void setTurret(float turretSetPoint);
	
	void update();
	
	static const float elevatorUpPower = -1.f;
	static const float elevatorDownPower = 1.f;	


	static const float blowIntakePower = 1.f;
#ifdef DUMPER
	static const float blowElevatorPower = 1.0;
#else
	static const float blowElevatorPower = -0.5;
#endif
	
	static const float suckIntakePower = -1.f;
#ifdef DUMPER
	static const float suckElevatorPower = -1.f;
#else
	static const float suckElevatorPower = 1.f;
#endif
	
#ifdef FAIL
	static const float slowSuckPower = -.31f;
#endif
#ifdef EPIC
	static const float slowSuckPower = -.62f;
#endif
	
	static const float firingShooterSpeed = -1.f;
	static const float firingElevatorSpeed = -1.f;
	
	// Should probably be tuned
#ifdef FAIL
	static const float prepareToFireIntakePower = -.31f;
#endif
#ifdef EPIC
	static const float prepareToFireIntakePower = -.4f;
#endif
	
	void enable();
	void disable();
};

extern RobotActions* robotActions;


int run_robotActions(...);
