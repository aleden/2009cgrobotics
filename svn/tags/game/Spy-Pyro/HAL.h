#pragma once
#include "RoboticsFramework.h"
#include "CANJaguar.h"
#include "WPILib.h"

#ifdef PYRO
#define DEFJAG CANJaguar
#else
#define DEFJAG CANJaguar
#endif

class HAL : public Part
{
public:
	// rStick is NOT the joystick on the right (and lStick.) Refer to the ports
	Joystick rStick; Joystick lStick;
#if 1
	Joystick hStick; // For hanger operator
#endif
private:
	AnalogChannel kickerAnalogChannel;
	//Left motors currently thought to be positive=forward.
	DEFJAG frMotor; DEFJAG flMotor; DEFJAG brMotor; DEFJAG blMotor;
	CANJaguar winchShootMotor;
	
	//Relay winchBrake;
	/*DEFJAG winchHangMotor; DEFJAG winchHangMotor2; */ // hanger is dead
	DEFJAG suckerMotor, suckerMotor2;
	Compressor compressor;
	
	DigitalInput kickLimit;
	Solenoid kickerSolenoid, hangerBreak, kickerForwardSolenoid;
	Encoder frEncoder, flEncoder,brEncoder, blEncoder;
	
	DriverStationLCD* dsLCD;
	Servo armServo;

	static const int rStickPort = 3; static const int lStickPort = 2;
	static const int hStickPort = 1;
	static const int frMotorPort = 3; static const int flMotorPort = 4;
	static const int brMotorPort = 9; static const int blMotorPort = 2;
	static const int armRelayChannel = 1;
	static const int kickerRelayChannel = 3;
	static const int winchShootMotorPort = 6;
#ifndef PYRO
	static const int armServoPort = 1;
#else
	static const int armServoPort = 1;
#endif
	//static const int winchBrakeRelayChannel = 2;
	static const int suckerMotorPort2 = 7;
	static const int suckerMotorPort = 8;
	static const int gyroPort = 1;
	static const int winchHangMotorPort = 7; // Retract - negative
	static const int winchHangMotor2Port = 11; // Retract - positive
	//encoder ports subject to change
	static const int frEncoderPort1 = 3; static const int frEncoderPort2 = 4;
	static const int flEncoderPort1 = 7; static const int flEncoderPort2 = 8;
	static const int brEncoderPort1 = 5; static const int brEncoderPort2 = 6;
	static const int blEncoderPort1 = 1; static const int blEncoderPort2 = 2;
	
	static const int pressureSwitchChannel = 9;
	static const int compressorRelayChannel = 1;
	
	static const int kickerForwardSolenoidChannel = 1;
	static const int kickerSolenoidChannel = 2;
	static const int hangerBreakChannel = 3; // TODO
	static const int kickLimitPort = 10;
	
	static const int kickerAnalogChannelPort = 3;
	
	float brMotorPower;
	float blMotorPower;
	float frMotorPower;
	float flMotorPower;
	float winchShootPower;
	float winchHangPower;
	float suckerPower;
	float gyroValue;
	float armServoAngle;
	bool kickerForwardValue;
	bool kickerValue;
	bool armValue;
	bool kickLimitValue;
	bool hangerBreakValue;
	//Relay::Value winchBrakeValue;
	int scriptMode;
	
	bool shootWinchBreaker;
	static const float shootWinchBreakerCurrent = 13;
	
public:
	static const int latchLockedAngle = 100, latchUnlockedAngle = 180;
	HAL() : Part("hal"),
	
	rStick(rStickPort), lStick(lStickPort),
#if 1
	hStick(hStickPort),
#endif
	kickerAnalogChannel(kickerAnalogChannelPort),
	frMotor(frMotorPort), flMotor(flMotorPort), brMotor(brMotorPort), blMotor(blMotorPort),
	winchShootMotor(winchShootMotorPort),
	//winchBrake(winchBrakeRelayChannel),
	//winchHangMotor(winchHangMotorPort),
	//winchHangMotor2(winchHangMotor2Port),
	suckerMotor(suckerMotorPort /*,CANJaguar::kCurrent*/),
	suckerMotor2(suckerMotorPort2 /*,CANJaguar::kCurrent*/),
	compressor(pressureSwitchChannel, compressorRelayChannel),
	kickLimit(kickLimitPort),
	kickerSolenoid(kickerSolenoidChannel),
	hangerBreak(hangerBreakChannel),
	kickerForwardSolenoid(kickerForwardSolenoidChannel),
#if 0
	gyro(gyroPort),
#endif
	frEncoder(frEncoderPort1, frEncoderPort2, true),
	flEncoder(flEncoderPort1, flEncoderPort2, true),
	brEncoder(brEncoderPort1, brEncoderPort2),
	blEncoder(blEncoderPort1, blEncoderPort2, true),
	dsLCD(DriverStationLCD::GetInstance()),
	armServo(armServoPort),
	kickerForwardValue(false)
	{
		compressor.Start();
		brMotorPower = 0;
		blMotorPower = 0;
		frMotorPower = 0;
		suckerPower = 0;
		gyroValue = 0;
		winchShootPower = 0.0;
		kickerValue = Relay::kOff;
		armValue = kickerValue = hangerBreakValue = false;
		//winchBrakeValue = Relay::kOff;
		scriptMode = 0;
		armServoAngle = latchLockedAngle;// l0ck
		frEncoder.Start();
		flEncoder.Start();
		brEncoder.Start();
		blEncoder.Start();
		shootWinchBreaker = false;
	}
	

private:
	//Internal Function--------------------------------------------------------
	void updateValues();
		
public:
	//Functions----------------------------------------------------------------
	void initOperator();
	void initAutonomous();
	void runOperator();
	void runAutonomous();
	void runDiagnostics();
	
	bool getWinchBreaker() { return shootWinchBreaker; }
	
	void setFRPower(float pwr);
	void setFLPower(float pwr);
	void setBRPower(float pwr);
	void setBLPower(float pwr);
	void setSuckerPower(float pwr);
	void setKickerLatch(bool value);
	bool getKickerLatch();
	void setWinchShootPower(float pwr);
	void setWinchHangPower(float pwr); // Positive - Retract
	void setArm(bool value);
	void setHangerBreak(bool value);
	void setArmServoAngle(float angle);
	void setKickerForwardValue(bool value);
	bool getArm();
	bool getKickLimit() { return kickLimitValue; }
	//void setWinchBrake(Relay::Value value);
	
	//These return the power that the wheel currently has.
	float getFRPower();
	float getFLPower();
	float getBRPower();
	float getBLPower();
	float getSuckerPower();
	float getWinchShootPower();
	float getWinchHangPower();
	float getArmServoAngle();
	
	float getSuckerCurrent() { return (suckerMotor.GetOutputCurrent() + suckerMotor2.GetOutputCurrent()) / 2.0; }

	//These give the power that the wheel is set to become. Names subject to change.
	float getNewFRPower();
	float getNewFLPower();
	float getNewBRPower();
	float getNewBLPower();
	float getNewSuckerPower();
	bool getNewKickerValue();
	float getNewWinchShootPower();
	float getNewWinchHangPower();
	//Relay::Value getNewWinchBrakeValue();
	//Relay::Value getNewArmValue();
	
	int getFREncoder();
	int getFLEncoder();
	int getBREncoder();
	int getBLEncoder();
	
	float getForward();
	float getStrafe();
	float getTurn();
	bool getLeftTrigger();
	bool getRightTrigger();
	
	float getGyroValue();
	void resetGyro();
	void resetEncoders();
	
	float getShooterCurrent();
	
	//added by rohan for diagnostic purposes
	float getWinchFPCurrent();
	float getWinchCIMCurrent();
	
	//for script object- do not delete
	bool getOverBumpButton();
	bool getWinchHangButton();
	bool getKickerButton();
	bool getScriptAbortButton();
	
	void stopMotors();
};

extern HAL* const hal;
