#include "WPILib.h"
#include "drive.h"
#include "wrappers.h"
#include "autotarget.h"
#include "DriverStation.h"

DriveRoutines::DriveRoutines()
{
	mtrSmoothTimer = new Timer();
	mtrSmoothTimer->Start();
}

DriveRoutines::~DriveRoutines()
{}

float DriveRoutines::trevorCorrect(float desiredWheel, float omniVel, bool isRightSide)
{
	float voltage = DriverStation::GetInstance()->GetBatteryVoltage();
	// convert desiredWheel into a desired voltage
	float desiredVoltage = desiredWheel * 12;
	
	// get magic konstants
	
	float maxRange;
	if (isRightSide)
		maxRange = 2.268;
	else
		maxRange = 2.245;
	
	
	
	float Ke;
	if (isRightSide)
		Ke = 0.2691;
	else
		Ke = 0.2797;
	
	// Convert wheel speed into radians per second
	/*float radiansPerVel = (1.0 / (hal->omniDiameter * M_PI)) * // rpm
	                      2 * M_PI;                            // radians per revolution*/
	float radiansPerVel = 2.0 / hal->omniDiameter;
	float radiansPerSec = radiansPerVel * omniVel;
	
	float maintainVelVoltage = radiansPerSec * Ke; // voltage needed for no acceleration
	
	// clamp
	float maxVoltage = maintainVelVoltage + maxRange;
	float minVoltage = maintainVelVoltage - maxRange;
	if (desiredVoltage > 0.0) desiredVoltage = maxVoltage;
	if (desiredVoltage < 0.0) desiredVoltage = minVoltage;
	
	// convert desired voltage into PWM and return
	return desiredVoltage / voltage;
}



 // TODO: test driving
float DriveRoutines::pidCorrect(float vel, float velW, float desiredWheel, 
		float currentWheel, double tdelta, CatlinPID &pid,bool doprint)
{
	pid.setpoint = vel;
	//pid.p = ((hal->getJoystickThrottle(J_1) + 1.0) / 2.0) * 0.03;
	const float pidK = 0.02;
	const float pidP = 0.16;
	pid.p = pidK / 2;
	//pid.i = 2 * pidK / pidP;
	pid.i = 0.0;
	pid.d = pidK * pidP / 16;
	//pid.d = 0.0;
	//printf("p=%f\n",pid.p);
	float delta = pid.calc(velW,tdelta);
	
	if (velW == 0)
		return desiredWheel;
	
	float lambda = abs((velW - vel) / velW);

	const float maxLambda = 0.3;
	
	static int ptimer = 0;
	if (doprint) {
		if (ptimer > 10) {
			/*printf("-------------------------\n");
			printf ("setpoint = %f\n", pid.setpoint);
			printf ("velW = %f\n",velW);
			printf ("delta = %f\n",delta);
			printf ("currentWheel = %f\n",currentWheel);
			printf ("--------------------------\n");
			printf("lambda = %f\n",lambda);*/
			ptimer = 0;
		}
		ptimer ++;
	}
	
	if (lambda > maxLambda)
		return desiredWheel + delta;
		//return desiredWheel;
	else
		return desiredWheel;
	
	float retval = currentWheel + delta;
	


	
	return retval;
	
	/*if (velW == 0) // prevent divide by zero
		return clamp(desiredWheel, 0.2);
	
	float slipK = (velW - vel) / velW;
	
	pid.setpoint = 0; // we want to have a slip of 0
	float mdelta = pid.calc(slipK,tdelta);
	return mdelta + desiredWheel;
	float idealWheel = clamp(currentWheel + mdelta);
	
	// opposite signs: return idealWheel
	if (desiredWheel * idealWheel < 0)
		return idealWheel;
	
	// check to see which is less torque
	if (abs(desiredWheel) < abs(idealWheel))
		return desiredWheel;
	else
		return idealWheel;*/
	return 0.0;
}

float DriveRoutines::asteroids(float desiredWheel, float currentWheel, float tdelta)
{
	const float asteroidsK = ((hal->getJoystickThrottle(J_1) + 1.0) / 2.0);
	return currentWheel + desiredWheel * tdelta * asteroidsK;
}

float DriveRoutines::pushMode(float desiredWheel, float omniVel, float maxSpeed, float timeDelta)
{
	float omniPWMPower = omniVel / maxSpeed; // pwm setting to go at omni velocity
	
	const float maxMtrChangePerSecond = 10.0 * ((hal->getJoystickThrottle(J_1) + 1.0) / 2.0); // from 0 to 5.0
	float maxMtrDelta = maxMtrChangePerSecond * timeDelta; // increase mtr speed more if more time passed
	
	return omniPWMPower + desiredWheel * maxMtrDelta; // joysticks control acceleration
}

float DriveRoutines::correctForSlip(float transVel, float omniVel, float desiredWheel, float maxSpeed, double time)
{
	float omniPWMPower = omniVel / maxSpeed; // pwm setting to go at omni velocity
	
	//float maxMtrDelta = 1.0 * ((hal->getJoystickThrottle(J_1) + 1.0) / 2.0); // from 0 to 1.0
	//const float maxMtrChangePerSecond = 10.0; // experimentally determined
	//float maxMtrDelta = maxMtrChangePerSecond * time;
	float maxMtrDelta = 0.653543; // tuned for failbase w/o trailer, should resemble epic
	//cout << "max = " << maxMtrDelta << endl;
	
	
	//DriverStation* ds = DriverStation::GetInstance(); // will have the dig inputs attached to driver station
	// we subtract from maxMtrDelta the difference between omniVel and transVel
	// the idea is that, the closer they are, the more you should be able to accelerate
	//const float difPerDelta = 0.004 * ((ds->GetAnalogIn(1) - 200) / 600.0);
	//cout << "dif = " << difPerDelta << endl;
	const float difPerDelta = 0.004; // CAUTION: This konstant may cause extreme PWNAGE
	maxMtrDelta -= difPerDelta * abs(transVel - omniVel);
	const float minMaxMtrDelta = 0.1;
	if (maxMtrDelta < minMaxMtrDelta)
		maxMtrDelta = minMaxMtrDelta;

	float retval;
	if (desiredWheel > omniPWMPower + maxMtrDelta) {
		retval = omniPWMPower + maxMtrDelta;
	} else if (desiredWheel < omniPWMPower - maxMtrDelta) {
		retval = omniPWMPower - maxMtrDelta;
	} else {
		retval = desiredWheel;
	}
	
	return retval;
}

void DriveRoutines::drive(float x, float y, bool smoothp, bool gyrop)
{	
	
	/*
	static int printTimer = 0;
	printTimer++;
	float right = hal->getJoystickThrottle(J_1);
	float left = -hal->getJoystickThrottle(J_1);
	
	hal->setMotorRight(right);
	hal->setMotorLeft(left);
	*/		
	float supposedLeft  = hal->getWheelVelocity(SIDE_LEFT,  TRANSMISSION);
	float supposedRight = hal->getWheelVelocity(SIDE_RIGHT, TRANSMISSION);
	
	/*
	if(printTimer%7 == 0) printf("left wheel velocity: %f, right wheel velocity: %f\n", supposedLeft, supposedRight);
	if(printTimer%7 == 0) printf("left wheel pwm:      %f, right wheel velocity: %f\n", left, right);
	return;
	*/
	
	float left, right;
	
	const double minTDelta = 0.05; // update 20 times per second
	double tdelta = mtrSmoothTimer->Get();
	if (tdelta < minTDelta) {
		return;
	}

#ifndef DUMPER
	x += autotargeting->turnOffset;
#endif

	mtrSmoothTimer->Reset();	
	
	static CatlinPID pidLeft (correctP,correctI,correctD,0.0),
	                 pidRight(correctP,correctI,correctD,0.0);
	
	// deadzone for x only
	// this is for the gyroCorrect, which expects perfectly straight driving
	const float deadzone = 0.1;
	if (abs(x) < deadzone) x = 0.0;
	
	if (gyrop) {
		x += gyroCorrect(x,y);
		//cout << "gyro correct: " << x << endl;
	}

	right  =  clamp(y + x);
	left = -clamp(y - x);
	
	
	const float maxLeftSpeedMinus = -121;
	const float maxLeftSpeedPlus = 121;
	const float maxRightSpeedMinus = -121;
	const float maxRightSpeedPlus = 121;
	
	float maxLeftSpeed;
	float maxRightSpeed;
	
	if (left < 0)  maxLeftSpeed  = maxLeftSpeedMinus;
	else           maxLeftSpeed  = maxLeftSpeedPlus;
	if (right < 0) maxRightSpeed = maxRightSpeedMinus;
	else           maxRightSpeed = maxRightSpeedPlus;
	
	float actualLeft  = hal->getWheelVelocity(SIDE_LEFT,  OMNI);
	float actualRight = hal->getWheelVelocity(SIDE_RIGHT, OMNI);
	
	if (smoothp && !hal->getJoystickTrigger(J_1))
	{
		left  = correctForSlip(supposedLeft, actualLeft, left,120,tdelta);
		right = correctForSlip(supposedRight, actualRight, right,120,tdelta);
		/*left = pidCorrect(actualLeft, supposedLeft,
				left, hal->getMotorLeft(), tdelta, pidLeft,true);
		right = pidCorrect(actualRight, supposedRight,
				right, hal->getMotorRight(), tdelta, pidRight,false);*/
		//left = trevorCorrect(left,actualLeft,false);
		//right = trevorCorrect(right,actualRight,true);
		//left = asteroids(left,hal->getMotorLeft(),tdelta);
		//right = asteroids(right,hal->getMotorRight(),tdelta);
	}
	
	hal->setMotorLeft(left);
	hal->setMotorRight(right);
}

float DriveRoutines::gyroCorrect(float x, float y)
{
	static int timer = 0;
	const float maxGyroCorrect = 0.4;
	
	static bool wasGoingStraight = false;
	static float startGyroHeading;
	
	bool isGoingStraight = (x == 0 && y != 0);
	
	if (isGoingStraight && !wasGoingStraight)
		startGyroHeading = hal->getAngle();
	
	float correction = 0.0;
	float desired = startGyroHeading;
	float current = hal->getAngle();
	float error = current - desired;
	
	if (isGoingStraight)
		correction = -abs(y) * clamp(error * getGyroCorrectConstant(), maxGyroCorrect);
	if (timer > 5) {
		timer = 0;
		// cout << "straight: " << isGoingStraight << " desired: " << desired << " current: " << current << endl;
	}
	timer++;
	
	wasGoingStraight = isGoingStraight;
	return correction;
}

float DriveRoutines::getGyroCorrectConstant()
{
	// ~5.0 seems to work very well on the fail base
	return 5.0;
	//return (hal->getJoystickThrottle(J_1) + 1.0) / 2.0 * 5.0; // 0-5
}

float DriveRoutines::getSmoothCorrectConstant()
{
	return (hal->getJoystickThrottle(J_1) + 1.0) / 2.0 + 1.0;
}

float DriveRoutines::clamp(float x, float range)
{
	if (x > range)
		return range;
	else if(x < -range)
		return -range;
	else
		return x;
}




