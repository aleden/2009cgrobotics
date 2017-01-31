#include "RobotActions.h"
#include "wrappers.h"
#include "Turret.h"
#include "compile config.h"

RobotActions::RobotActions()
{
	mutex = semMCreate(SEM_Q_PRIORITY | SEM_INVERSION_SAFE | SEM_DELETE_SAFE);
	firing = outaking = manualElevatorUp = manualElevatorDown = emptyCellCollecting = preFiring = false;
	mode = BALL_OFF;
	fireSpeed = 1.0; // default firing speed
	enabled = true;
	preFireTimer.Start();
}

RobotActions::~RobotActions()
{
	semDelete(mutex);
}

void RobotActions::enable()
{
	CRITICAL_REGION(mutex)
	enabled = true;
	END_REGION
}
void RobotActions::disable()
{
	CRITICAL_REGION(mutex)
	enabled = false;
	END_REGION
}

// TODO: do the RobotActions stuff
void RobotActions::prepareToFire()
{
	CRITICAL_REGION(mutex)
	//if (!wasPreparingToFire)
		if (mode != BALL_READY_TO_FIRE)
			mode = BALL_PREPARE;
	END_REGION
	/*
	// Should probably be tuned
	const float prepareToFireIntakePower = 0.5f;
	hal->setElevatorGoal(-1.f);
	hal->setIntakeGoal(prepareToFireIntakePower);
	*/
}

void RobotActions::stopFiring()
{ firing = false; preFiring = false; }

void RobotActions::stopBlowing()
{ outaking = false; }

void RobotActions::elevatorUp()
{ manualElevatorUp = true; manualElevatorDown = false; }

void RobotActions::elevatorDown()
{ manualElevatorDown = true; manualElevatorUp = false; }

void RobotActions::elevatorStop()
{ manualElevatorDown = manualElevatorUp = false; }

void RobotActions::fire(float fireSpeed)
{
	CRITICAL_REGION(mutex)
	//firing = true;
	if (!firing && !preFiring) {
		if (fireSpeed == hal->lowShooterSpeed) {
			firing = true;
		} else {
			preFiring = true;
		}
		preFireTimer.Reset();
	}
	this->fireSpeed = fireSpeed;
	END_REGION
}

// this function sucks balls into the hopper
void RobotActions::suck()
{
	CRITICAL_REGION(mutex)
	mode = BALL_COLLECT;
	END_REGION
	/*
	const float suckIntakePower = -1.0f;
	const float suckElevatorPower = 1.0f;
	
	hal->setElevatorGoal(suckElevatorPower);
	hal->setIntakeGoal(suckIntakePower);
	*/
}

void RobotActions::slowIntake() {
	CRITICAL_REGION(mutex)
	emptyCellCollecting = true;
	END_REGION
}

void RobotActions::stopSlowIntake() {
	CRITICAL_REGION(mutex)
	emptyCellCollecting = false;
	END_REGION
}

void RobotActions::blow()
{
	CRITICAL_REGION(mutex)
	outaking = true;
	END_REGION
	
	/*
		hal->setElevatorGoal(blowElevatorPower);
		hal->setIntakeGoal(blowIntakePower);
	*/
}

void RobotActions::off()
{
	CRITICAL_REGION(mutex)
	mode = BALL_OFF;
	//preparingToFire = firing = intaking = outaking = false;	
	END_REGION
}

void RobotActions::update()
{
	CRITICAL_REGION(mutex)
	
	if (!enabled) return;
	
	float elevatorSpeed = 0.f, intakerSpeed = 0.f, shooterSpeed = 0.f;
	
	if (mode == BALL_PREPARE)
	{
		elevatorSpeed = -1.f;
		intakerSpeed = prepareToFireIntakePower;
		//intakerSpeed = 0.5 * (hal->getJoystickThrottle(J_1) - 1) / 2.0;
		
#ifndef DUMPER
		if (hal->getWobbleState()) // if wobble is tripped
			mode = BALL_READY_TO_FIRE;
#endif
		//hal->setElevatorGoal(-1.f);
		//hal->setIntakeGoal(prepareToFireIntakePower);
	}
	
	if (mode == BALL_READY_TO_FIRE) {
		elevatorSpeed = 0.0;
		//shooterSpeed = hal->getManualShooterSpeed(); // warm up the shooter
	}
	
	if (mode == BALL_COLLECT)
	{
		elevatorSpeed = suckElevatorPower;
		intakerSpeed  = suckIntakePower;
	}
	
	if (preFiring)
	{
		shooterSpeed = hal->maxShooterSpeed;
		
		const float preFireTime = 0.1;
		if (preFireTimer.Get() > preFireTime) {
			preFiring = false;
			firing = true;
		}
	}
	
	if (firing)
	{
		shooterSpeed = fireSpeed;
		elevatorSpeed = firingElevatorSpeed;
	}
	
	if (outaking)
	{
		elevatorSpeed = blowElevatorPower;
		intakerSpeed  = blowIntakePower;
	}
	
	if(emptyCellCollecting)
	{
		intakerSpeed = slowSuckPower;
	}
	
	if (manualElevatorUp) { elevatorSpeed = -1.f; }
	if (manualElevatorDown) { elevatorSpeed = 1.f; }
	
	hal->setElevatorGoal(elevatorSpeed);
	hal->setIntakeGoal(intakerSpeed);
	hal->setMotorSpeed(M_SHOOTER, shooterSpeed);
	
	END_REGION
}

void RobotActions::setTurret(float turretSetPoint)
{
#ifndef DUMPER
	const float degreesPerPot = -90.0 / 340.0;
	const float centerPot = 520;
	
	turret->setToAngle((turretSetPoint - centerPot) * degreesPerPot);
#endif
}

int run_robotActions(...)
{
	while (true)
	{
		robotActions->update();
	}
}
