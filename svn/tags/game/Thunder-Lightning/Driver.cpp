#include "Driver.h"
#include "deadzone.h"
#include "CameraControl.h"
#include "ArmControl.h"
#include "Utils.h"
#include "UserInterface.h"
#include <math.h>

#define SHORT_AUTONOMOUS_MODE

Driver::Driver(ChickenRobot* chicken, CameraControl* cameraControl, ArmControl* armControl) :
	Control(chicken), cameraControl(cameraControl), armControl(armControl), waitStateEndState(DAS_OFF),
	waitStateTime(0.0f)
{
	timer.Start();
	
    DEAD_ZONE = .1;
    CUBIC_MOTOR_ADJUSTMENT = .6;
#if CHICKEN_ROBOT == CHICKEN_THUNDER
	ENCODER_CLICKS_PER_METER = 573.0f;
#elif CHICKEN_ROBOT == CHICKEN_LIGHTNING
	ENCODER_CLICKS_PER_METER = 112.9411765f;
#endif
	LANE_LENGTH = 5.9436;
    SIDE_OF_FORK = .7874;
    LENGTH_OF_FORK = 2.1336;
    DISTANCE_TO_FORK = 3.81;
    ARM_LENGTH = 1.811;
    TESTING_DISTANCE = 2.4;
    currentSpeedLeft = currentSpeedRight = 0.0f;
    START_VISION_PROCESSING = LANE_LENGTH - ARM_LENGTH;
    
    inLineFollowMode = false;
    inImageTrackingMode = false;
    turning = false;
    backingUp = false;
    lastLeftSignal = false;
    isFork = false;
    atFork = false;

    t = new Timer();
    t2 = new Timer();
    t->Reset();
    t2->Reset();
    
    leftJaguarA = addJaguar(LEFT_JAGUARA_PORT, "LEFT A");
	leftJaguarB = addJaguar(LEFT_JAGUARB_PORT, "LEFT B");
	rightJaguarA = addJaguar(RIGHT_JAGUARA_PORT, "RIGHT A");
	rightJaguarB = addJaguar(RIGHT_JAGUARB_PORT, "RIGHT B");

    shiftersSolenoid = addSolenoid(SHIFTERS_SOLENOID_PORT, "SHIFTERS");
    lineSensorSolenoid = addSolenoid(LINE_SENSOR_SOLENOID_PORT, "LINE SENSOR SOLENOID");

	leftJaguarEncoder = addEncoder(LEFT_JAGUAR_ENCODER_PORTA, LEFT_JAGUAR_ENCODER_PORTB, "LEFT");
	rightJaguarEncoder = addEncoder(RIGHT_JAGUAR_ENCODER_PORTA, RIGHT_JAGUAR_ENCODER_PORTB, "RIGHT");
	
	rangefinder = addAnalogChannel(RANGEFINDER_PORT, "ULTRASONIC SENSOR");
	
	leftJaguarEncoder->Start();
	rightJaguarEncoder->Start();
	lineSensorSolenoid->Set(true);
    
    left = addDigitalInput(14,"LEFT SENSOR");
    middle = addDigitalInput(12, "MIDDLE SENSOR");
    right = addDigitalInput(13, "RIGHT SENSOR");
#if CHICKEN_ROBOT == CHICKEN_LIGHTNING
    inout = addDigitalInput(7, "inout autonomous switch");
    leftright = addDigitalInput(9, "leftright autonomous switch");
    middlehigh = addDigitalInput(10, "middlehigh autonomous switch");
    onoff = addDigitalInput(11, "onoff autonomous switch");
    isIn = inout->Get();
    isLeft = leftright->Get();
    isMiddle = middlehigh->Get();
    isOn = onoff->Get();
#else
    isIn = false; // line
    isLeft = false;
    isMiddle = false; // middle/high peg
    isOn = true;
#endif
    
    testing = false;
}

int Driver::average(int val, int val2)
{ return (val + val2)/2; }

int Driver::getEncoderClicks(float meters)
{ return (int)(meters * ENCODER_CLICKS_PER_METER); }

void Driver::autonomousInit()
{
#if CHICKEN_ROBOT == CHICKEN_LIGHTNING
	isIn = inout->Get(); // is 
    isLeft = leftright->Get();
    isMiddle = middlehigh->Get();
    isOn = onoff->Get();
#endif
    armControl->setAutonomousPreset(isIn, isLeft, isMiddle, isOn);

	cameraControl->setIsHighPeg(!isMiddle);
	cameraControl->setImageProcess(false);
	
	resetEncoders();
	shiftDown();
	setAutonomousState(isOn ? DAS_INITIALIZE : DAS_OFF);
	
	turningRight = true;
	zigzagCounter = 0;
	
	timer.Start();
	timer.Reset();
}

void Driver::resetEncoders()
{
	leftJaguarEncoder->Reset();
	rightJaguarEncoder->Reset();
}

void Driver::printAutonomousStatus()
{
#if 0
	static const char* autonomousStateStrings[] = 
	{
		"DAS_OFF",
		"DAS_INITIALIZE",
		"DAS_FIND_LINE",
		"DAS_TRACK_LINE",
		"DAS_FIRST_TURN",
		"DAS_GO_STRAIGHT",
		"DAS_SECOND_TURN",
		"DAS_VISION_PROCESS_ALIGN",
		"DAS_ARM_CONTROL",
		"DAS_BACK_UP"
	};
	DriverStationLCD* lcd = DriverStationLCD::GetInstance();
	lcd->Printf(DriverStationLCD::kUser_Line5,1,"autostate = %s", autonomousStateStrings[getAutonomousState()]);
	lcd->UpdateLCD();
	printf("autostate = %s\n", autonomousStateStrings[getAutonomousState()]);
#endif
}

void Driver::goForwardVerySlow()
{
    setFrontRight(-.18);
    setFrontLeft(-.18);
    setBackRight(-.18);
    setBackLeft(-.18);
}
void Driver::turnSharplyLeftCamera()
{
    setFrontRight(-.42);
    //setFrontLeft(0.0f);
    setBackRight(-.42);
    //setBackLeft(0.0f);
}

void Driver::turnSharplyRightCamera()
{
	//setFrontRight(0.f);
    setFrontLeft(-.42);
    //setBackRight(0.f);
    setBackLeft(-.42);
}

void Driver::autonomousPeriodic()
{
	if (!isOn)
		return;

	printEncoderStatus();
	printUltrasonicStatus();
	
	int leftEnc = leftJaguarEncoder->Get();
	int rightEnc = rightJaguarEncoder->Get();
	int avg = (int)average(fabs(rightEnc), fabs(leftEnc));
#ifdef WOBBLE
	int turnDistance = 95;
#else
	int turnDistance = 0;
#endif
#if CHICKEN_ROBOT == CHICKEN_LIGHTNING
	turnDistance = turnDistance * 0.8095;
#endif
	int secondTurnDistance = 20;
	int distance = 2100 - 100/*500*/; // 216 inches
	int backingDistance = 300;
	int valueToTurn = 160;
	int valueToScore = 79;
	int backTurnDistance = turnDistance + secondTurnDistance + 20;
	switch (getAutonomousState())
	{
	case DAS_INITIALIZE:
		timer.Reset();
		timer.Start();
		resetEncoders();
		setAutonomousState(DAS_FIND_LINE);
		break;
		
		
	// Zig-zag until line is found
	case DAS_FIND_LINE:
		if (middle->Get())
		{
			stop();
			timer.Stop();
			timer.Reset();
			resetEncoders();
			setAutonomousState(DAS_TRACK_LINE);
		}
		else
		{
			if (timer.Get() > 1.0 + (0.5 * zigzagCounter))
			{
				timer.Reset();
				turningRight = !turningRight;
				zigzagCounter++;
			}
			if (turningRight)
			{
				turnRight();
			}
			else turnLeft();
		}
		break;
		
	case DAS_TRACK_LINE:
		printf("Left encoder clicks: %d\nRight encoder clicks: %d\nAvg: %d\nGoal: %d\n", leftEnc, rightEnc, avg, distance);
		if (rangefinder->GetValue() <= valueToTurn)
		{
			stop();
			resetEncoders();
			setAutonomousState(DAS_TURN);
		}
		else
		{
			StraightLineFollower();
		}
		break;
			
	case DAS_TURN:
		printf("Left encoder clicks: %d\nRight encoder clicks: %d\nAvg: %d\nGoal: %d\n", leftEnc, rightEnc, avg, distance);
		if (fabs(rightEnc) >= turnDistance)
		{
			stop();
			resetEncoders();
			setAutonomousState(DAS_FORWARD);
		}
		else
		{
			leftJaguarA->Set(-0.8);
			leftJaguarB->Set(-0.8);
		}
		break;

	case DAS_FORWARD:
		printf("Left encoder clicks: %d\nRight encoder clicks: %d\nAvg: %d\nGoal: %d\n", leftEnc, rightEnc, avg, distance);
		if (rangefinder->GetValue() <= valueToScore)
		{
			stop();
			timer.Reset();
			timer.Start();
			armControl->lowerArmSmallAmount();
			setAutonomousState(DAS_SCOREA);
		}
		else
		{
			goForwardSlow();
		}
		break;
	
	// Opens claw
	case DAS_SCOREA:
		if (timer.Get() > 1.0)
		{
			armControl->openClaw();
			
			timer.Reset();
			setAutonomousState(DAS_SCOREB);
		}
		break;
	
	// Lowers claw away from tube
	case DAS_SCOREB:
	{
		if (timer.Get() > 1.0)
		{
			armControl->lowerArmSmallAmount();

			timer.Reset();
			setAutonomousState(DAS_LOWER_ARM);
		}
		break;
	}
	
	// Turn right away from tube
	case DAS_LOWER_ARM:
		if (timer.Get() > 1.0)
		{
			resetEncoders();
			turnSharplyRight();

			timer.Reset();
			setAutonomousState(DAS_TURN_AGAIN);
		}
		break;
		
	case DAS_TURN_AGAIN:
		printf("Left encoder clicks: %d\nRight encoder clicks: %d\nAvg: %d\nGoal: %d\n", leftEnc, rightEnc, avg, distance);
		if (fabs(rightEnc) >= secondTurnDistance)
		{
			stop();

			timer.Reset();
			resetEncoders();
			setAutonomousState(DAS_BACK_UP);
		}
		else
		{
			rightJaguarA->Set(0.0);
			rightJaguarB->Set(0.0);
			leftJaguarA->Set(-0.8);
			leftJaguarB->Set(-0.8);
		}
		break;
		
	case DAS_BACK_UP:
		if (avg >= 100)
		{
			stop();
			setAutonomousState(DAS_RETRACT_ARM);
		}
		else
		{
			backUp();
		}
		break;

	case DAS_RETRACT_ARM:
		armControl->closeElbow();
		armControl->setArmPID(18);
		resetEncoders();
		setAutonomousState(DAS_TURN_LEFT);
		break;

	case DAS_TURN_LEFT:
		printf("FUCKING TURNING THE FUCK LEFT\n");
		printf("Left encoder clicks: %d\nRight encoder clicks: %d\nAvg: %d\nGoal: %d\n", leftEnc, rightEnc, avg, distance);
		if (fabs(rightEnc) >= backTurnDistance)
		{
			stop();
			resetEncoders();
			setAutonomousState(DAS_BACKUPFINAL);
		}
		else
		{
			rightJaguarA->Set(0.0);
			rightJaguarB->Set(0.0);
			leftJaguarA->Set(0.8);
			leftJaguarB->Set(0.8);
		}
		break;
		
	case DAS_BACKUPFINAL:
		if (avg >= 550)
		{
			stop();
			setAutonomousState(DAS_OFF);
		}
		else
		{
			backUp();
		}
		break;
		
	case DAS_OFF:
		break;
	}
}

void Driver::setFrontRight(float setSpeed) {
    rightJaguarA->Set(-setSpeed);
}

void Driver::setFrontLeft(float setSpeed) {
    leftJaguarA->Set(setSpeed);
}

void Driver::setBackRight(float setSpeed) {
    rightJaguarB->Set(-setSpeed);
}

void Driver::setBackLeft(float setSpeed) {
    leftJaguarB->Set(setSpeed);
}

float Driver::deadZone(float motorSpeed) {
    if (fabs(motorSpeed) < DEAD_ZONE) {
        return 0;
    }
    return motorSpeed;
}

float Driver::powerOf(float base, int toPowerOf) {
    float result = base;
    for (int i = 1; i < toPowerOf; i++) {
        result *= base;
    }
    return result;
}

float Driver::adjustMotorFunction(float adjustment, float joystickValue, int typeOfFunction)
{
    // adjustment is the amount between the type of function and linear
    // joystickValue is what you're changing (the joystick value)
    // typeOfFunction i.e. cubed, power of 5 (so 3,5,7,9 depending on the curve you want)
    return ((adjustment*powerOf(joystickValue,typeOfFunction)) + ((1-adjustment)*joystickValue));
}

void Driver::driveCode()
{
    if (false) {
        CUBIC_MOTOR_ADJUSTMENT = getJoystickThrottle(UserInterface::PILOT_JOYSTICK_PORT);
    }

    float lSpeed = 0.0;
	float rSpeed = 0.0;
	
	float joystickX = adjustMotorFunction(CUBIC_MOTOR_ADJUSTMENT, -deadzone(getJoystickX(1)),3);
    float joystickY = adjustMotorFunction(CUBIC_MOTOR_ADJUSTMENT, deadzone(getJoystickY(1)),3);

	lSpeed += joystickX;
	rSpeed += joystickX;

	lSpeed += joystickY;
	rSpeed -= joystickY;
	
	//calculateNewSetpoint(lSpeed, rSpeed);
	
	leftJaguarA->Set(lSpeed);
	leftJaguarB->Set(lSpeed);

	rightJaguarA->Set(rSpeed);
	rightJaguarB->Set(rSpeed);
}

void Driver::teleopInit()
{
	leftJaguarA->Set(0.0f);
	leftJaguarB->Set(0.0f);

	rightJaguarA->Set(0.0f);
	rightJaguarB->Set(0.0f);
}

void Driver::teleopPeriodic()
{
	PRESSED_TRIGGER(isShiftTrigger, getJoystickTrigger(UserInterface::PILOT_JOYSTICK_PORT));
	if (isShiftTrigger)
		shift(!isShifted());

	driveCode();
	
	if (getJoystickRawButton(1, 11))
	{
		resetEncoders();
	}
	
	printEncoderStatus();
	printUltrasonicStatus();
}

void Driver::printUltrasonicStatus()
{
	DriverStationLCD::GetInstance()->Printf(DriverStationLCD::kUser_Line5, 1, "us:%d", rangefinder->GetValue());
}

void Driver::printEncoderStatus()
{
	int leftEnc = leftJaguarEncoder->Get();
	int rightEnc = rightJaguarEncoder->Get();

	DriverStationLCD::GetInstance()->Printf(DriverStationLCD::kUser_Line2, 1, "avg:%d", abs(average(leftEnc, rightEnc)));
	DriverStationLCD::GetInstance()->Printf(DriverStationLCD::kUser_Line3, 1, "le:%d", leftEnc);
	DriverStationLCD::GetInstance()->Printf(DriverStationLCD::kUser_Line4, 1, "re:%d", rightEnc);
}

void Driver::printLineSensorsStatus()
{

#if 0
	char leftTrueString[] = "L: 1";
	char leftFalseString[] = "L: 0";
	char middleTrueString[] = "M: 1";
	char middleFalseString[] = "M: 0";
	char rightTrueString[] = "R: 1";
	char rightFalseString[] = "R: 0";


	printf(left->Get() ? leftTrueString : leftFalseString);
	printf(middle->Get() ? middleTrueString : middleFalseString);
	printf(right->Get() ? rightTrueString : rightFalseString);

	print(left->Get() ? leftTrueString : leftFalseString);
	print(middle->Get() ? middleTrueString : middleFalseString);
	print(right->Get() ? rightTrueString : rightFalseString);
#endif
}
void Driver::turnLeft()
{
    setFrontRight(-.5);
    setFrontLeft(0);
    setBackRight(-.5);
    setBackLeft(0);
}

void Driver::turnSharplyLeft()
{
    setFrontRight(-.5);
    setFrontLeft(.5);
    setBackRight(-.5);
    setBackLeft(.5);
}

void Driver::turnRight()
{
    setFrontRight(0);
    setFrontLeft(-.5);
    setBackRight(0);
    setBackLeft(-.5);
}

void Driver::turnSharplyRight()
{
    setFrontRight(.5);
    setFrontLeft(-.5);
    setBackRight(.5);
    setBackLeft(-.5);
}

void Driver::stop()
{
    setFrontRight(0);
    setFrontLeft(0);
    setBackRight(0);
    setBackLeft(0);
}

void Driver::goStraight()
{
    setFrontRight(-1.0);
    setFrontLeft(-1.0);
    setBackRight(-1.0);
    setBackLeft(-1.0);
}

void Driver::backUp()
{
    setFrontRight(1.0);
    setFrontLeft(1.0);
    setBackRight(1.0);
    setBackLeft(1.0);
}

void Driver::backUpVerySlow()
{
    setFrontRight(.3);
    setFrontLeft(.3);
    setBackRight(.3);
    setBackLeft(.3);
}

void Driver::goForwardSlow()
{
    setFrontRight(-.35);
    setFrontLeft(-.35);
    setBackRight(-.35);
    setBackLeft(-.35);
}

void Driver::goForwardMedium()
{
	setFrontRight(-.6);
    setFrontLeft(-.6);
    setBackRight(-.6);
    setBackLeft(-.6);
}

void Driver::StraightLineFollower()
{
    if (middle->Get())
    {
    	goForwardMedium();
    }
    else
    {
    	if (left->Get()) {turnRight(); lastLeftSignal = true;}
        else if (right->Get()) {turnLeft(); lastLeftSignal = false;}
        else
        {
            if (lastLeftSignal) turnRight();
            else turnLeft();
        }
    }
    //Wait(0.04);
}

void Driver::ForkLineFollower()
{
    if (middle->Get())
    {
    	goForwardSlow();
    }
    else
    {
    	if (left->Get() && right->Get()) {atFork = true; stop(); }
    	else if (left->Get()) {turnSharplyRight(); lastLeftSignal = true;}
        else if (right->Get()) {turnSharplyLeft(); lastLeftSignal = false;}
        else
        {
            if (lastLeftSignal) turnSharplyRight();
            else turnSharplyLeft();
        }
    }
    //Wait(0.04);
}

void Driver::calculateNewSetpoint(float leftSpeed, float rightSpeed) {
	static const float increment = .01f;
	static const float deadzone = .1f;
	
    currentSpeedLeft = leftJaguarA->Get();
    currentSpeedRight = rightJaguarA->Get();
    
    if ((currentSpeedRight > 0 && rightSpeed < .1) || (currentSpeedRight < 0 && rightSpeed > -.1)) {
    	if (currentSpeedRight > rightSpeed) {
    		currentSpeedRight -= increment;
    		if (currentSpeedRight < rightSpeed) {
    			currentSpeedRight = rightSpeed;
    		}
    		if (currentSpeedRight < deadzone && currentSpeedRight > rightSpeed) { 
    			currentSpeedRight = rightSpeed;
    		}
    	}

    	else if (currentSpeedRight < rightSpeed) {
    		currentSpeedRight += increment;
    		if (currentSpeedRight > rightSpeed) {
    			currentSpeedRight = rightSpeed;
    		}
    		if (currentSpeedRight > -deadzone && currentSpeedRight < rightSpeed) {
    			currentSpeedRight = rightSpeed;
    		}
    	}
    } else {
    	currentSpeedRight = rightSpeed;
    }
    
    if ((currentSpeedLeft > 0 && leftSpeed < .1) || (currentSpeedLeft < 0 && leftSpeed > -.1)) {
         if (currentSpeedLeft > leftSpeed) {
        	 currentSpeedLeft -= increment;
        	 if (currentSpeedLeft < leftSpeed) {
        		 currentSpeedLeft = leftSpeed;
        	 }
        	 if (currentSpeedLeft < deadzone && currentSpeedLeft > leftSpeed) {
        		 currentSpeedLeft = leftSpeed;
        	 }
         }

         else if (currentSpeedLeft < leftSpeed) {
            currentSpeedLeft += increment;
            if (currentSpeedLeft > leftSpeed) {
            	currentSpeedLeft = leftSpeed;
            }
            if (currentSpeedLeft > -deadzone && currentSpeedLeft < leftSpeed) {
            	currentSpeedLeft = leftSpeed;
            }
         }

         
    } else {
         currentSpeedLeft = leftSpeed;
    }
}
