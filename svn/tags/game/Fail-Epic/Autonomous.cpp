#include "Autonomous.h"
#include "Turret.h"
#include "drive.h"
#include "camera.h"
#include "AutoTarget.h"
#include "wrappers.h"
#include "DriverStationLCD.h"
#include "util.h"
#include "robotActions.h"
#include "compile config.h"

void AutonomousRoutines::setMode(const AutonomousParams& params)
{
	mode = AM_ZIGZAG; // default
		if (params.top && params.right && params.left)
			mode = AM_ZIGZAG;
		if (params.top && params.right && !params.left)
			mode = AM_SPIN;
		if (params.top && !params.right && params.left)
			mode = AM_COLE;
		if (!params.top && !params.right && !params.left)
			mode = AM_POOF;
		if (!params.top && !params.right && params.left)
			mode = AM_POOF_LEFT;
		if (!params.top && params.right && params.left)
			mode = AM_EMPTYCOLLECT_LEFT;
		if (!params.top && params.right && !params.left)
			mode = AM_EMPTYCOLLECT;
	
		DriverStationLCD::GetInstance()->Printf(DriverStationLCD::kUser_Line3,1,
					"t = %i l = %i r = %i",(int)params.top, (int)params.left, (int)params.right);
				DriverStationLCD::GetInstance()->UpdateLCD();
}

AutonomousRoutines::AutonomousRoutines(const AutonomousParams& params):
	pid(kp, ki, kd, 0)
{
	
	//cout << "params: " << params.top << " " << params.right << " " << params.left << endl;
	setMode(params);
	//cout << " mode = " << mode << endl;
}
AutonomousRoutines::~AutonomousRoutines()
{}

void AutonomousRoutines::run_autonomous()
{
	if (DriverStation::GetInstance()->IsDisabled()) {
		const int top = 6;
    	const int left = 8;
    	const int right = 7;
    	
    	DriverStation* sdriver = DriverStation::GetInstance(); 
    	bool t = sdriver->GetDigitalIn(top);
    	bool r = sdriver->GetDigitalIn(right);
    	bool l = sdriver->GetDigitalIn(left);
    	
    	setMode(AutonomousParams(t,r,l));
    	DriverStationLCD::GetInstance()->Printf(DriverStationLCD::kMain_Line6,1,
    						"mode = %i",mode);
    					DriverStationLCD::GetInstance()->UpdateLCD();
    	//cout << "mode = " << mode << endl; 
		return;
	}
	
	//cout << "e mode = " << mode << endl; 
	
	/*if(mode == AM_ZIGZAG)
		amZigzag();
	else if(mode == AM_FOWLER)
		amFowler();
	else if(mode == AM_PROPORTIONAL)
		amProportional();
	else if(mode == AM_SPIN)
		amSpin();
	else if(mode == AM_COLE)
		amCole();
	else if (mode == AM_POOF)
		amPoof();
	else if (mode == AM_POOF_LEFT)
		amPoofLeft();
	else if (mode == AM_EMPTYCOLLECT)
		amEmptyCollect();
	else if (mode == AM_EMPTYCOLLECT_LEFT)
		amEmptyCollectLeft();*/
	
	amEmptyCollect();
}

void AutonomousRoutines::amEmptyCollect()
{
	robotActions->slowIntake();
	hal->setIntakeGoal(-0.6);
	amPoof(false);
}

void AutonomousRoutines::amEmptyCollectLeft()
{
	robotActions->slowIntake();
	hal->setIntakeGoal(-0.6);
	amPoofLeft(false);
}


// designed to obliterate the Cheesy Poofs
void AutonomousRoutines::amPoof(bool turnIn)
{
	enum POOF_STATE { PREP, TWIST, BOOKIT, TWIST_2, PREP_2, STOP4KENT};
	
	static POOF_STATE state = PREP;
	static Timer *timer = NULL;
	
	static int startLeft, startRight;
	
	if (timer == NULL) { timer = new Timer; timer->Start(); }
	
	if (state == PREP) {
		driver->drive(0.0,-1.0);
		
		// check to see if we've gone forward "enough"
		if (timer->Get() > 0.5)
			state = TWIST;
	} else if (state == TWIST) {
		driver->drive(1.0,-0.3);
		//cout << ang << endl;
		if (timer->Get() > 1.5) {
			state = BOOKIT;
			startLeft = hal->getWheelDistance(SIDE_LEFT, OMNI);
			startRight = hal->getWheelDistance(SIDE_RIGHT, OMNI);
		}
	} else if (state == BOOKIT) {
		int dl = abs(startLeft - hal->getWheelDistance(SIDE_LEFT, OMNI));
		int dr = abs(startRight - hal->getWheelDistance(SIDE_RIGHT, OMNI));
		int avg = (dl + dr) / 2;
		
		int bookit_distance = 24 * 12 - 87 * 2; // distance traveled: 24 feet across - 87 inches turn per side
		//if (!turnIn)
		//	bookit_distance += 12;
		
		
		if (avg > bookit_distance) {
			/*if (!turnIn) state = STOP4KENT; else*/ state = TWIST_2;
			timer->Reset();
		}
		
		driver->drive(0.0,-1.0);
	} else if (state == TWIST_2) {
		driver->drive(1.0,0.2); // turn and slam on the brakes
		//cout << ang << endl;
		if (timer->Get() > 1.0) {
			state = PREP_2;
		}
	} else if (state == PREP_2) {
		driver->drive(0.0,-1.0);
				
		// check to see if we've gone forward "enough"
		if (timer->Get() > 1.5)
			state = STOP4KENT;
	} else { driver->drive(0.0,0.0); }
}

// designed to obliterate the Cheesy Poofs, but turns left
void AutonomousRoutines::amPoofLeft(bool turnIn)
{
	enum POOF_STATE { PREP, TWIST, BOOKIT, TWIST_2, PREP_2, STOP4KENT};
	
	static POOF_STATE state = PREP;
	static Timer *timer = NULL;
	
	static int startLeft, startRight;
	
	if (timer == NULL) { timer = new Timer; timer->Start(); }
	
	if (state == PREP) {
		driver->drive(0.0,-1.0);
		
		// check to see if we've gone forward "enough"
		if (timer->Get() > 0.5)
			state = TWIST;
	} else if (state == TWIST) {
		driver->drive(-1.0,-0.3);
		//cout << ang << endl;
		if (timer->Get() > 1.5) {
			state = BOOKIT;
			startLeft = hal->getWheelDistance(SIDE_LEFT, OMNI);
			startRight = hal->getWheelDistance(SIDE_RIGHT, OMNI);
		}
	} else if (state == BOOKIT) {
		int dl = abs(startLeft - hal->getWheelDistance(SIDE_LEFT, OMNI));
		int dr = abs(startRight - hal->getWheelDistance(SIDE_RIGHT, OMNI));
		int avg = (dl + dr) / 2;
		
		int bookit_distance = 24 * 12 - 87 * 2; // distance traveled: 24 feet across - 87 inches turn per side
		//if (!turnIn) bookit_distance += 12;
		
		if (avg > bookit_distance) {
			/*if (!turnIn) state = STOP4KENT; else*/ state = TWIST_2;
			timer->Reset();
		}
		
		driver->drive(0.0,-1.0);
	} else if (state == TWIST_2) {
		driver->drive(-1.0,0.2); // turn and slam on the brakes
		//cout << ang << endl;
		if (timer->Get() > 1) {
			state = PREP_2;
		}
	} else if (state == PREP_2) {
		driver->drive(0.0,-1.0);
				
		// check to see if we've gone forward "enough"
		if (timer->Get() > 1.5)
			state = STOP4KENT;
	} else { driver->drive(0.0,0.0); }
}

void AutonomousRoutines::amCole() // based on Cole's video he showed us
{
	static bool inited = false;
	static Timer *timer = NULL;
	
	if (timer == NULL) { timer = new Timer; timer->Start(); }
	
	enum COLE_STATE { WEAK_TURN, JACKKNIFE, FORWARD };
	
	static COLE_STATE state = WEAK_TURN;
	
	const float weakTurnTime = 1.0;
	const float jackTime = 0.9;
	
	if (state == WEAK_TURN) {
		driver->drive(-0.1,-1.0);
		if (timer->Get() > weakTurnTime) {
			state = JACKKNIFE;
			timer->Reset();
		}
	}
	if (state == JACKKNIFE) {
		driver->drive(1.0,-0.2);
		if (timer->Get() > jackTime) {
			state = FORWARD;
		}
	}
	if (state == FORWARD) {
		driver->drive(0.0,-1.0);
	}
}

// KEVIN CODE
// at this point, all of the autotracking has been enabled
// All we do is drive forward (book it!) and then do more driving
// We hope to score by randomly shooting and stuff
void AutonomousRoutines::amZigzag()
{
	const float driveForwardT = 1.3;
	const float stopT = 0.2;
	const float turnT = 0.6;
	
	enum AUTO_STATE { DRIVE_FORWARD, STOP1, TURN, STOP2 };
	
	static AUTO_STATE state = DRIVE_FORWARD;
	
	static Timer *timer = NULL;
	
	static bool hasGoneForwardYet = false;
	
	if (timer == NULL) { timer = new Timer; timer->Start(); }

#ifndef DUMPER
	autotargeting->enableAutoRotate();
	autotargeting->enableAutoPan();
	autotargeting->enableAutoFire();
#endif
	
	//static int foo = 0;
	//if (foo++ % 5000 == 0) {
	//	printf("turretSpeed = %f\n",hal->getMotorSpeed(M_TURRET));
	//}
	
	float x,y;
	
	static bool turnPos = false;
	
	if (state == DRIVE_FORWARD) {
		x = 0.0; y = -0.5;
		if (timer->Get() > driveForwardT) {
			timer->Reset();
			state = TURN;
		}
	}
	
	/*if (state == STOP1) {
		x = y = 0.0;
		if (timer->Get() > driveForwardT) {
			timer->Reset();
			state = TURN;
		}
	}*/
	
	if (state == TURN) {
		if (turnPos) {
			x = -1.0;
			y = -0.5;
		} else {
			x = 1.0;
			y = -0.5;
		}
		if (timer->Get() > turnT) {
			timer->Reset();
			state = DRIVE_FORWARD;
			turnPos = !turnPos;
		}
	}
	
	/*if (state == STOP2) {
		x = y = 0.0;
		if (timer->Get() > stopT) {
			timer->Reset();
			state = DRIVE_FORWARD;
			hasGoneForwardYet = true;
		}
	}*/
#ifndef DUMPER
	if (autotargeting->isTargetAquired && hasGoneForwardYet)
	{
		if(autotargeting->isReadyToFire)
			driver->drive(0.0, 0.0);
		else
			driver->drive(0.0,-0.3);
	}
	else
		driver->drive(x,y);
#else
	driver->drive(x,y);
#endif
}

// YALE CODE
// (autonomous code adapted from Fowler's)
void AutonomousRoutines::amFowler()
{
	// Drive forward
	// for(int i = 0; i < escapeTime; i++)
		// driver->drive(0, 1, false, false);
	
	// Seek target
	static float previousX = 0;
	turret->setToAngle(0);
	robotTrack(previousX);
}

// JOHNNY CODE
void AutonomousRoutines::amProportional()
{
	/* enum DriveState {
		BEGINNING, COLLISION, NORMAL
	};
	enum TargetingState {
		NO_TARGET_FOUND, FAR_FROM_TARGET, NEAR_TARGET, IN_RANGE, READY_TO_FIRE // probably unneceasary
	};
	static DriveState driveState = BEGINNING;
	static TargetingState targetingState = NO_TARGET_FOUND;
	static Timer driveTimer = new Timer();
	
	// determine driveState:
	float leftTransVel = hal->getWheelVelocity(SIDE_LEFT, TRANSMISSION);
	float rightTransVel = hal->getWheelVelocity(SIDE_RIGHT, TRANSMISSION);
	float leftOmniVel = hal->getWheelVelocity(SIDE_LEFT, OMNI);
	float rightOmniVel = hal->getWheelVelocity(SIDE_RIGHT, OMNI);
	
	float avgTransVel = (rightTransVel + leftTransVel)/2;
	float avgOmniVel = (rightOmniVel + leftOmniVel)/2;
	const float collisionVelDiffBuffer = 70.0;
	if(abs(avgTransVel - avgOmniVel) > collisionVelDiffBuffer) driveState = COLLISION;
	
	//when locked on:
	float angleOff = autotargeting->getTargetAngle() + turret->getAngle(); // aimed right, target angle +, turret angle -
	//angleOff: + means target to left, - means target to right
	
	
	
	float distanceAway;
	float x;
	float y;
	cameraMan->getTarget(x, y, distanceAway);
	if(autotargeting->inRange()) state = IN_RANGE;
	else if(distanceAway < 9.0) state = NEAR_TARGET; */
}

/* TODO: autonomous robot tracking
 * get ready to shoot
 * turret uses PID to focus on target
 * robot uses PID (based on turret's angle) to hone in on target
 * detect collisions?
 * find if closer than or as close as maxDistance
 * if turret is not turned too much to fire: fire
 * else don't fire (or stop firing)
*/

void AutonomousRoutines::amSpin()
{
	static Timer *timer = NULL;
	
	if (timer == NULL) { timer = new Timer; timer->Start(); }
	
	enum SPIN_STATE { SPIN_ESCAPE, SPIN_STRAIGHT, SPIN_SPIN };
	
	static SPIN_STATE state = SPIN_ESCAPE;
	
	const float escapeTime = 2.5;
	const float spinTime = 0.5;
	const float straightTime = 0.25;
	
	if (state == SPIN_ESCAPE) {
		if (timer->Get() < escapeTime) {
			timer->Reset();
			state = SPIN_SPIN;
			cout << "transitioning to SPIN_SPIN" << endl;
		}
		driver->drive(0.0,-1.0);
	}
	
	if (state == SPIN_SPIN) {
		if (timer->Get() < spinTime) {
			timer->Reset();
			state = SPIN_STRAIGHT;
			cout << "transitioning to SPIN_STRAIGHT" << endl;
		}
		driver->drive(1.0,-0.3);
	}
	
	if (state == SPIN_STRAIGHT) {
		if (timer->Get() < straightTime) {
			timer->Reset();
			state = SPIN_SPIN;
			cout << "transitioning to SPIN_SPIN" << endl;
		}
		driver->drive(0.0,-1.0);
	}
	
	/*
	static Timer timer;
	static bool initialized = false;
	
	if (!initialized) {
		initialized = true;
		timer.Start();
	}
	
	if(timer.Get() < 2)
		driver->drive(-1, 0);
	else
	{
		// Check if robot is stuck
		if(abs(hal->getWheelVelocity(SIDE_LEFT,  TRANSMISSION)) < 1 ||
				abs(hal->getWheelVelocity(SIDE_RIGHT, TRANSMISSION)) < 1)
			driver->drive(1, 0);
		else
			driver->drive(0, 1);
	}*/
}

// Old function
void AutonomousRoutines::robotTrack(float &previousX)
{
	float x = 0, y = 0, d = 0, xSpeed, ySpeed, panSpeed = 0.35;
	if(cameraMan->getTarget(x, y, d))
	{
		previousX = x;
		xSpeed = pid.calc(x, dt);
		
		if(xSpeed > 1)
			xSpeed = 1;
		else if(xSpeed < -1)
			xSpeed = -1;
		printf("xSpeed is: %f\n", xSpeed);
		
		if(d > minDistance)
			ySpeed = yAdjustSpeed;
		else
			ySpeed = 0;
		
		driver->drive(xSpeed, ySpeed, false, false);
		Wait(dt);
	}
	else // pan to look for target
	{
		if(previousX > 0) // Pan to the right
			driver->drive(panSpeed, 0, false, false);
		else // Pan to the left
			driver->drive(-panSpeed, 0, false, false);
	}
}

