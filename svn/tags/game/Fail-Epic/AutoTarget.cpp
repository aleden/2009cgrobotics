#include "WPILib.h"
#include "Turret.h"
#include "camera.h"
#include "wrappers.h"
#include "util.h"
#include "AutoTarget.h"
#include "Math.h"
#include "RobotActions.h"
#include "DriverStationLCD.h"
#include "OI.h"
#include "util.h"
#include "compile config.h"

int run_autotarget(...)
{
	autotargeting = new AutoTarget();
	autotargeting->run();
	
	return 0;
}


AutoTarget::AutoTarget() :
	lockonPID(kp,ki,kd,0.0)
{
	imageTimer.Start();
	autopan = autofire = panIncreaseP = isAutoFiring = autorotate = hadImage = false;
	
	panLimit = 30;
	panCenter = 0;
}


void AutoTarget::run()
{
	bool isFiring = false;
	float shooterSpeed = -1.0;
	
	Timer shootTimer;
	shootTimer.Start();
	
	const float shootTime = 0.5; // shoot for 3/4 seconds when you see it
	
	for(;;)
	{
		float newTurnOffset = 0.0;
		// No image found; x and y are swapped, as it's sideways
		if(!cameraMan->getTarget(lastY,lastX,lastD))
		{
			//cout << "Can't find image!" << endl;
			lockonPID.reset();
			if(autopan && !isFiring)
				run_autopan();
			
			isReadyToFire = false;
			isTargetAquired = false;
			hadImage = false;
			
		}
		// Image found
		else
		{
			//const float extrapolateK = 3.0 * (hal->getJoystickThrottle(J_1) + 1.0) / 2.0; // extend one "delta" forward
			//float xdelta = lastX - previousX;
			//float predictedX = extrapolateK * xdelta + lastX;
			
			hadImage = true;
			isTargetAquired = true;
			
			float realX = findRealX();
			
			bool isAligned = aligned(realX); // is the turret lined up for a shot
			bool isInRange = inRange(); // is the shooter in range of the target
			bool isAngleLegit = isTurretAngleLegit(); // can we shoot from the current turret angle
			
			if (autopan) {
				if (!isAligned && !isFiring)
					run_lockon();
#ifndef DUMPER
				else
					hal->setMotorSpeed(M_TURRET,0.0);
#endif
			}
			DriverStationLCD::GetInstance()->Printf(DriverStationLCD::kUser_Line4,1,
				"lastX = %f",lastX);
			DriverStationLCD::GetInstance()->UpdateLCD();
			
			//if (!isAligned) {
			//	if (autopan)
			//		run_lockon();
			//} else { // we are aligned; stop the turret -or- see if we should pan anyways to keep up
			//	if (autopan) {
			//		hal->setMotorSpeed(M_TURRET,0.0);
					//run_lockon();
					
					
					// we don't have a past image
					/*if (!hadImage) {
						hal->setMotorSpeed(M_TURRET,0.0);
					} else {
						hal->setMotorSpeed(M_TURRET,0.0);
						const float extrapolateK = 10.0 * (hal->getJoystickThrottle(J_1) + 1.0) / 2.0;
						float xdelta = realX - lastRealX;
						float predictedX = extrapolateK * xdelta + realX;
						
						//DriverStationLCD::GetInstance()->Printf(DriverStationLCD::kUser_Line2,1,
						//	"predictedX = %f",predictedX);
						//DriverStationLCD::GetInstance()->Printf(DriverStationLCD::kUser_Line6,1,
						//		"realX = %f",realX);
						//DriverStationLCD::GetInstance()->UpdateLCD();
						printf("k = %f d = %f p = %f",extrapolateK,xdelta,predictedX);
						
						if (aligned(predictedX)) {
							hal->setMotorSpeed(M_TURRET,0.0);
							printf(" => stop\n");
						} else {
							// preemptive panning
							if (predictedX > 0) panIncreaseP = true;
							else                panIncreaseP = false;
							run_autopan();
							printf(" => preemptive\n");
						}
					}*/
			//	}
			//}
			
			if (isAligned && isInRange && isAngleLegit) {
				if (autofire) {
					shooterSpeed = calcShooterSpeed();
					isFiring = true;
					shootTimer.Reset();
					isAutoFiring = true;
				}
				isReadyToFire = true;
			} else {
				isReadyToFire = false;
			}
			
			if (autorotate && isAligned && isInRange && !isAngleLegit)
				newTurnOffset = rotateForTarget();
			
			lastRealX = realX;
		}
		
		imageTimer.Reset();
		
		// timer expired
		if (shootTimer.Get() > shootTime) {
			isFiring = false;
			isAutoFiring = false;
			//cout << "Timer expired." << endl;
		}
		
		if (isFiring) {
			robotActions->fire(shooterSpeed);
#ifndef DUMPER
			hal->setMotorSpeed(M_TURRET,0.0);
#endif
			//cout << "*Actually* firing..." << endl;
		} else {
			// make sure that the OI isn't telling it to fire
			if (DriverStation::GetInstance()->IsOperatorControl())
				if (DriverStation::GetInstance()->GetDigitalIn(OIRoutines::iFireChannel) != OIRoutines::iFireTrue)
					robotActions->stopFiring();
		}
		//cout << "---------------------------" << endl;
		
		turnOffset = newTurnOffset;
	}
}

void AutoTarget::run_autopan()
{
//#ifdef FAIL
	const float panSpeed = 0.1640625;
//#endif
//#ifdef EPIC
	//const float panSpeed = 0.25;
//	const float panSpeed = 0.25 * (hal->getJoystickThrottle(J_1) + 1.0) / 2.0;
//#endif
	//const float panSpeed = (hal->getJoystickThrottle(J_1) + 1.0)/4; // must be tuned
	// check bounds of panning
	float currentAngle = hal->getTurretAngle();
	if (currentAngle > (panLimit + panCenter)) panIncreaseP = false;
	if (currentAngle < (panCenter - panLimit)) panIncreaseP = true;
	
	// move the turret
#ifndef DUMPER
	if (panIncreaseP)
		hal->setMotorSpeed(M_TURRET,panSpeed);
	else
		hal->setMotorSpeed(M_TURRET,-panSpeed);
#endif
	
	//cout << "panIncreaseP = " << panIncreaseP << endl;
	
	//cout << "Pan speed: " << panSpeed << endl;
}

float AutoTarget::getTargetAngle() { // from camera in radians counter-clockwise is positive
	return getTargetAngle(lastX);
}

float AutoTarget::getTargetAngle(float x) { // from camera in radians counter-clockwise is positive
	return .3297 * x + .08357; // lin reg line, in real angle
}

float AutoTarget::findRealX() { // positive: target to the left of aim, aimed to the right
	float targetAngle = getTargetAngle();
	float sinTargetAngle = sin(targetAngle);
	return sinTargetAngle * lastD;
}

float AutoTarget::findRealX(float x) { // positive: target to the left of aim, aimed to the right
	float targetAngle = getTargetAngle(x);
	float sinTargetAngle = sin(targetAngle);
	return sinTargetAngle * lastD;
}

bool AutoTarget::isTurretAngleLegit()
{
	float currentTurretAngle = hal->getTurretAngle();			// angle of turret
	return currentTurretAngle >= maxShootingAngleNeg && currentTurretAngle <= maxShootingAnglePos;
}

void AutoTarget::run_lockon()
{
	//const float maxPanSpeed = 0.25;
	//if (lastX > 0) panIncreaseP = true;
	//else           panIncreaseP = false;
	
	
	// you know what?
	// fuck pid
	//lockonPID.p = 0.5 * (hal->getJoystickThrottle(J_1) + 1.0) / 2.0;
	/*lockonPID.p = 0.4 * (hal->getJoystickThrottle(J_2) + 1.0) / 2.0;
	lockonPID.i = 0.2 * (hal->getJoystickThrottle(J_1) + 1.0) / 2.0;
	lockonPID.d = 0.0;
	cout << "lockPID.p = " << lockonPID.p << endl;
	cout << "lockPID.i = " << lockonPID.i << endl;
	cout << "lockPID.d = " << lockonPID.d << endl;
	float delta = -lockonPID.calc(lastX, imageTimer.Get()); // *prtty* sure it's negative*/
	
	// check to make damn sure the sign is right
	// commented out for testing signs
	//if ((delta < 0 && panIncreaseP) || (delta > 0 && !panIncreaseP)) delta = -delta;
	
	// make sure that it doesn't go faster than we would otherwise pan
	//if (delta < -maxPanSpeed) delta = -maxPanSpeed;
	//if (delta >  maxPanSpeed) delta =  maxPanSpeed;
	
	//hal->setMotorSpeed(M_TURRET,delta);
	
	
	
	const float panSpeed = 0.1640625;
	// XXX this is cheap and dirty
	// However, it seems to work........
	// It's the same as autopan, but instead of checking for bounds it checks for where the target is
	if (lastX > 0) panIncreaseP = true;
	else           panIncreaseP = false;
	
	const float cutoffX = 0.34252;
	
#ifndef DUMPER
	if (fabs(lastX) < cutoffX) {
		hal->setMotorSpeed(M_TURRET,0.0);
	} else {
		// move the turret
		if (panIncreaseP)
			hal->setMotorSpeed(M_TURRET,panSpeed);
		else
			hal->setMotorSpeed(M_TURRET,-panSpeed);
	}
#endif
	// fuck cutoffs, too
	/*
	const float cutoffX = 0.2 * (hal->getJoystickThrottle(J_1) + 1.0) / 2.0;
	
	cout << "cutoffX = " << cutoffX << " panSpeed = " << panSpeed << endl;
	
	// check to see if the angle is close enough
	if (lastX < 0 && lastX > -cutoffX || lastX >= 0 && lastX < cutoffX) {
		hal->setMotorSpeed(M_TURRET,0.0);
	} else {
		// move the turret
		if (panIncreaseP)
			hal->setMotorSpeed(M_TURRET,panSpeed);
		else
			hal->setMotorSpeed(M_TURRET,-panSpeed);
	}*/
}

float AutoTarget::calcShooterSpeed()
{
	/*const float vMax = 30.66;
	const float fallTime = .1768; // time it takes before ball can't get over the trailer walls
	*/
	/*const float overShoot = .1; // in feet, to make range safer
	const float K = 1.0 / 5.0; // 1.0 PWM for 5 ft
	float shooterSpeed = (lastD + overShoot) * K;*/
	
	// realistically, it's so noisy that we should just overshoot unless it's really really close
	// ^ stupid. Trevor is stupid and rams into the fucking trailer, undershoot.
	if (lastD >= 5.2)
		return 1.0;
	if (lastD >= 4.0)
		return 0.8;
	return 0.6;
}

float AutoTarget::rotateForTarget()
{
	const float rotateSpeed = 0.4;
	float currentAngle = hal->getTurretAngle();
	
	if (currentAngle < maxShootingAngleNeg)
		return rotateSpeed;
	if (currentAngle > maxShootingAnglePos)
		return -rotateSpeed;
	return 0.0;
}

bool AutoTarget::inRange()
{
	return lastD < maxFiringDistance;
}

bool AutoTarget::aligned(float realX) //aligned for a shot in X direction
{
	const float maxRealXPos = 1.34;
	const float maxRealXNeg = -0.7; // max off in the negative direction
	
	DriverStationLCD::GetInstance()->Printf(DriverStationLCD::kUser_Line4,1,
		"distance = %f",lastD);
	DriverStationLCD::GetInstance()->Printf(DriverStationLCD::kUser_Line3,1,
			"realX = %f",realX);
	DriverStationLCD::GetInstance()->UpdateLCD();
	
	float checkX;
	if (realX < 0) checkX = maxRealXNeg;
	else           checkX = maxRealXPos;
	
	return abs(realX) < checkX;
}

void AutoTarget::enableAutoPan()
{
	turret->enablePIDControl = false;
	autopan = true;
}
void AutoTarget::disableAutoPan()
{
	turret->enablePIDControl = true;
	autopan = false;
}
void AutoTarget::enableAutoFire()
{
	turret->reset();
	autofire = true;
}
void AutoTarget::disableAutoFire()
{
	autofire = false;
}

void AutoTarget::enableAutoRotate()
{
	autorotate = true;
}

void AutoTarget::disableAutoRotate()
{
	autorotate = false;
}// this brace was closed we done by rohan