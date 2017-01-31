#pragma once
#include "WPILib.h"
#include "Control.h"

class CameraControl;
class ArmControl;

enum DRIVER_AUTONOMOUS_STATE
{
	DAS_INITIALIZE,
	DAS_FIND_LINE,
	DAS_TRACK_LINE,
	DAS_TURN,
	DAS_FORWARD,
	DAS_SCOREA,
	DAS_SCOREB,
	DAS_LOWER_ARM,
	DAS_TURN_AGAIN,
	DAS_BACK_UP,
	DAS_RETRACT_ARM,
	DAS_TURN_LEFT,
	DAS_BACKUPFINAL,
	DAS_OFF
};

// Drive code written by Benji, edited haphazardly by Casey
// Line follower code written by Casey
class Driver : public Control
{
public:	
	float DEAD_ZONE;
    float CUBIC_MOTOR_ADJUSTMENT;
    float ENCODER_CLICKS_PER_METER;
    float START_VISION_PROCESSING;
    float LANE_LENGTH;
    float SIDE_OF_FORK;
    float LENGTH_OF_FORK;
    float DISTANCE_TO_FORK;
    float ARM_LENGTH;
    float TESTING_DISTANCE;
    float currentSpeedLeft;
    float currentSpeedRight;
    Timer* t;
    Timer* t2;
    bool inLineFollowMode;
    bool inImageTrackingMode;
    bool turning;
    bool backingUp;
    bool lastLeftSignal;
    bool isFork;
    bool atFork;
    bool isIn;
    bool isLeft;
    bool isMiddle;
    bool isOn;
    bool testing;
    bool turningRight;
    int zigzagCounter;

private:
	static const unsigned int LEFT_JAGUARA_PORT = 3;
	static const unsigned int LEFT_JAGUARB_PORT = 6;

	static const unsigned int RIGHT_JAGUARA_PORT = 5;
	static const unsigned int RIGHT_JAGUARB_PORT = 4;

	static const unsigned int LEFT_JAGUAR_ENCODER_PORTA = 1;
	static const unsigned int LEFT_JAGUAR_ENCODER_PORTB = 2;

	static const unsigned int RIGHT_JAGUAR_ENCODER_PORTA = 3;
	static const unsigned int RIGHT_JAGUAR_ENCODER_PORTB = 4;

	static const unsigned int SHIFTERS_SOLENOID_PORT = 5;
	
	static const unsigned int LINE_SENSOR_SOLENOID_PORT = 8;
	
	static const unsigned int RANGEFINDER_PORT = 1;

    DRIVER_AUTONOMOUS_STATE autonomousState;

    Jaguar* leftJaguarA;
	Jaguar* leftJaguarB;
    Jaguar* rightJaguarA;
    Jaguar* rightJaguarB;
    
    Solenoid* shiftersSolenoid;
    Solenoid* lineSensorSolenoid;

    Encoder* leftJaguarEncoder;
    Encoder* rightJaguarEncoder;
    
    Timer timer;
    
    Compressor* compressor;
    
    DigitalInput* left;
    DigitalInput* middle;
    DigitalInput* right;
    
    AnalogChannel* rangefinder;
    
#if CHICKEN_ROBOT == CHICKEN_LIGHTNING
    DigitalInput* inout;
    DigitalInput* leftright;
    DigitalInput* middlehigh;
    DigitalInput* onoff;
#endif
    CameraControl* cameraControl;
    ArmControl* armControl;

    DRIVER_AUTONOMOUS_STATE waitStateEndState;
    float waitStateTime;
    Timer waitStateTimer;
    unsigned int waitStateID;
public:
    Driver(ChickenRobot* chicken, CameraControl* cameraControl, ArmControl* armControl);
    virtual ~Driver() {}
    
    void setFrontRight(float setSpeed);
    void setFrontLeft(float setSpeed);
    void setBackRight(float setSpeed);
    void setBackLeft(float setSpeed);
    float deadZone(float motorSpeed);
    static float powerOf(float base, int toPowerOf);
    float adjustMotorFunction(float adjustment, float joystickValue, int typeOfFunction);
    void driveCode();
    void teleopInit();
    void teleopPeriodic();
    void autonomousInit();
    void autonomousPeriodic();
    void turnLeft();
    void turnSharplyLeft();
    void turnRight();
    void turnSharplyRight();
    void stop();
    void goStraight();
    void Stop();
    void backUp();
    void backUpVerySlow();
    void goForwardSlow();
    void goForwardMedium();
    void goForwardVerySlow();
    void turnSharplyLeftCamera();
    void turnSharplyRightCamera();
    void StraightLineFollower();
    void ForkLineFollower();
	int average(int val, int val2);
	int getEncoderClicks(float meters);
	void calculateNewSetpoint(float leftSpeed, float rightSpeed);
	void resetEncoders();
	void printUltrasonicStatus();
	void printLineSensorsStatus();
	void printAutonomousStatus();
	void printEncoderStatus();

	inline void shiftUp()
	{ shift(true); }

	inline void shiftDown()
	{ shift(false); }
	
	inline void shift(bool isHigh)
	{ shiftersSolenoid->Set(isHigh); }
	
	inline bool isShifted()
	{ return shiftersSolenoid->Get(); }
	
	inline DRIVER_AUTONOMOUS_STATE getAutonomousState()
	{ return autonomousState; }
	
	inline void setAutonomousState(DRIVER_AUTONOMOUS_STATE state)
	{ autonomousState = state; }
};
