#include "HAL.h"
#include "WPILib.h"
#include "locator.h"
#include "control.h"

HAL* const hal = new HAL();

static float clamp(float x)
{
	if (x > 1.0) return 1.0;
	if (x < -1.0) return -1.0;
	return x;
}

void HAL::initAutonomous(void)
{
	//compressor.Start();
	#if 0
	frEncoder.Reset();
	flEncoder.Reset();
	brEncoder.Reset();
	blEncoder.Reset();
	gyro.Reset();
#endif
}

void HAL::stopMotors()
{
	setSuckerPower(0.f);
	setWinchShootPower(0.f);
	setWinchHangPower(0.f);
	setFLPower(0.f);
	setBLPower(0.f);
	setFRPower(0.f);
	setBRPower(0.f);
}

void HAL::initOperator(void)
{
	//compressor.Start();
#if 0
	frEncoder.Reset();
	flEncoder.Reset();
	brEncoder.Reset();
	blEncoder.Reset();
	gyro.Reset();
#endif
}

void HAL::runAutonomous(void)
{
	updateValues();
}

void HAL::runOperator(void)
{
	updateValues();
}

void HAL::runDiagnostics(void)
{
	updateValues();
}


void HAL::updateValues()
{
	dsLCD->Printf(DriverStationLCD::kUser_Line1, 1, "DIAG=%d", (int) isDiagnosticsModeEnabled());
	dsLCD->Printf(DriverStationLCD::kUser_Line2, 1, "AUTO=%d", (int) control->getAutoMode());
	dsLCD->UpdateLCD();

	kickLimitValue = kickLimit.Get();
	//kickLimitValue = kickerAnalogChannel.GetAverageValue() > 250;
	//Right motors currently thought to require negation. Subject to change if found incorrect.
	//Wait(0.1);
	//printf("%f\t%f\t%f\t%f\n",frMotorPower,flMotorPower,brMotorPower,blMotorPower);
#if 1
	frMotor.Set(-clamp(frMotorPower));
	flMotor.Set(clamp(flMotorPower));
	brMotor.Set(-clamp(brMotorPower));
	blMotor.Set(clamp(blMotorPower));
#endif
	
	//return;
	static int tmr = 0;
	if (tmr++ > 10)
	{
		tmr = 0;
		//printf("kickLimit = %i\n",(int)kickLimitValue);
		//printf("suckerPower = %f\n",suckerPower);
		//printf("GetVoltage() = %f\nGetValue() = %d\nGetAverageValue() = %d\nGetAverageVoltage() = %f\n\n",
		//		kickerAnalogChannel.GetVoltage(), (int)kickerAnalogChannel.GetValue(),
		//		kickerAnalogChannel.GetAverageValue(), kickerAnalogChannel.GetAverageVoltage());
		//printf("hangerWinch = %f brake = %i\n",winchHangPower,(int)hangerBreakValue);
		//printf("updating values: %f %f %f %f\n",frMotorPower,flMotorPower,brMotorPower,blMotorPower);
		//printf("winch = %f\n",winchHangPower);
		//printf("solenoid = %i",armValue);
		//printf("enc: fr = %i fl = %i br = %i bl = %i\n",frEncoder.Get(),flEncoder.Get(),brEncoder.Get(),blEncoder.Get());
		//float x = winchShootMotor.GetOutputCurrent();
		//if (x != 0)
		//printf("%f %f %f\n",locator->getX(), locator->getY(), locator->getTheta());
		//	printf("%f\n",x);
		//printf("solenoid latch = %i\n",kickerValue);
	}
	//return;

#if 1
	suckerMotor.Set(suckerPower);
	suckerMotor2.Set(suckerPower);
#endif
	//armServo.SetAngle(armServoAngle);
	
#if 1
	// breaker control
	if (getShooterCurrent() > shootWinchBreakerCurrent) { shootWinchBreaker = true; printf("breaker tripped\n"); }
	if (winchShootPower > 0)
		shootWinchBreaker = false;
	
	//shootWinchBreaker = false; // debug
	
	if (!shootWinchBreaker) {
		winchShootMotor.Set(winchShootPower);
	} else { // "breaker" tripped
		if (winchShootPower > 0) // positive is OK when tripped
			winchShootMotor.Set(winchShootPower);
		else
			winchShootMotor.Set(0.0);
	}
	
	
	//winchHangMotor.Set(winchHangPower);
#ifdef SPY
	//winchHangMotor2.Set(-winchHangPower);
#else
	//const float kCurrentLimit = 1;
	//winchHangMotor2.Set(winchHangPower * kCurrentLimit);
#endif
#endif
	// Arm
	//armSolenoid.Set(armValue);
	kickerSolenoid.Set(kickerValue);

	kickerForwardSolenoid.Set(kickerForwardValue);
	//hangerBreak.Set(!hangerBreakValue);

	//winchBrake.Set(winchBrakeValue);
#if 0
	gyroValue = gyro.GetAngle();
#endif
}

//Positive should be clockwise. Inversion happens at update.
void HAL::setFRPower(float pwr) { frMotorPower = pwr; }
void HAL::setFLPower(float pwr) { flMotorPower = pwr; }
void HAL::setBRPower(float pwr) { brMotorPower = pwr; }
void HAL::setBLPower(float pwr) { blMotorPower = pwr; }
void HAL::setSuckerPower(float pwr)
{
#ifdef PYRO
	suckerPower = -pwr;
#else
	suckerPower = pwr;
#endif
}
void HAL::setWinchShootPower(float pwr) { winchShootPower = pwr; }
void HAL::setKickerLatch(bool value) { kickerValue = value; }
bool HAL::getKickerLatch() { return kickerValue; }
void HAL::setWinchHangPower(float pwr)
{
	//printf("setting winch power to %f",pwr);
	if (!hangerBreakValue) {
		winchHangPower = pwr;
	//	printf(", success\n");
	} else {
	//	printf(", fail\n");
	}
}

void HAL::setKickerForwardValue(bool value) { kickerForwardValue = value; }
void HAL::setArm(bool value) { armValue = value;}
void HAL::setArmServoAngle(float angle){ armServoAngle = angle;}
void HAL::setHangerBreak(bool value)
{
#ifdef PYRO
	hangerBreakValue = value;
	//setWinchHangPower(0.f); // who the FUCK wrote this?
#endif
}

bool HAL::getArm() { return armValue; }
//void HAL::setWinchBrake(Relay::Value value) {winchBrakeValue = value;}

//Change these if right motors are found to be positive=forward.
float HAL::getFRPower() { return -frMotor.Get(); }
float HAL::getFLPower() { return flMotor.Get(); }
float HAL::getBRPower() { return -brMotor.Get(); }
float HAL::getBLPower() { return blMotor.Get(); }
float HAL::getSuckerPower() {return suckerMotor.Get(); }
float HAL::getWinchShootPower() { return /*winchShootMotor.Get();*/ 0; }
float HAL::getWinchHangPower() { /*return winchHangMotor.Get();*/ return 0; }

float HAL::getNewFRPower() { return frMotorPower; }
float HAL::getNewFLPower() { return flMotorPower; }
float HAL::getNewBRPower() { return brMotorPower; }
float HAL::getNewBLPower() { return blMotorPower; }
float HAL::getNewSuckerPower() {return suckerPower; }
float HAL::getNewWinchShootPower() {return winchShootPower; }
bool HAL::getNewKickerValue() {return kickerValue; }
float HAL::getNewWinchHangPower() {return winchHangPower;}
//Relay::Value HAL::getNewWinchBrakeValue() {return winchBrakeValue;}

// joystick Y axis thought to require negation
float HAL::getForward() {return -lStick.GetY();}
float HAL::getStrafe() {return lStick.GetX();}
float HAL::getTurn() {return rStick.GetX();}
bool HAL::getLeftTrigger() {return lStick.GetTrigger();}
bool HAL::getRightTrigger() {return rStick.GetTrigger();}

int HAL::getFREncoder() { return frEncoder.Get(); }
int HAL::getFLEncoder() { return flEncoder.Get(); }
int HAL::getBREncoder() { return brEncoder.Get(); }
int HAL::getBLEncoder() { return blEncoder.Get(); }

#if 0
float HAL::getGyroValue() { return gyroValue; }
void HAL::resetGyro() { gyro.Reset(); }
void HAL::resetGyro() {
	gyro.Reset();
	}
#endif

//temporary
bool HAL::getOverBumpButton() {return false; }
bool HAL::getWinchHangButton() {return hStick.GetRawButton(4); }
bool HAL::getKickerButton() {return hStick.GetRawButton(2); }
bool HAL::getScriptAbortButton() {return hStick.GetRawButton(3); }

float HAL::getShooterCurrent() {
#if 1
	return winchShootMotor.GetOutputCurrent();
#else
	return 0;
#endif
}

//added by rohan for diagnostic purposes
float HAL::getWinchFPCurrent() { /*return winchHangMotor.GetOutputCurrent();*/ return 0; }
float HAL::getWinchCIMCurrent() { /*return winchHangMotor2.GetOutputCurrent();*/ return 0; }
float HAL::getArmServoAngle() {return armServo.GetAngle();}

float HAL::getGyroValue() { return 0; } // { return gyroValue; }
void HAL::resetGyro() { } // { gyro.Reset(); }
void HAL::resetEncoders()
{
	frEncoder.Reset();
	flEncoder.Reset();
	brEncoder.Reset();
	blEncoder.Reset();
}


