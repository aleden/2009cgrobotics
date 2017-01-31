#include "WPILib.h"

class GyroSample : public SimpleRobot
{
	Joystick *leftStick;
	Joystick *rightStick;
	Jaguar *frontLeftMotor;
	Jaguar *frontRightMotor;
	Jaguar *backLeftMotor;
	Jaguar *backRightMotor;
	int mode;

public:
	GyroSample(void)
	{
		GetWatchdog().SetEnabled(false);
		
		leftStick = new Joystick(0);
		rightStick = new Joystick(1);
		frontLeftMotor = new Jaguar(0);
		frontRightMotor = new Jaguar(1);
		backLeftMotor = new Jaguar(2);
		backRightMotor = new Jaguar(3);
		mode = 0;
	}
	void Autonomous(void)
	{
		
	}
	
	void OperatorControl(void)
	{
		if (leftStick->GetTrigger() || rightStick->GetTrigger()) {	// if emergency brake
			frontLeftMotor->Set(0);
			frontRightMotor->Set(0);
			backLeftMotor->Set(0);
			backRightMotor->Set(0);
		} else {
			if (mode == 0) {	// if mode is left: move/strafe & right: turn
				frontLeftMotor->Set(leftStick->GetY()*rightStick->GetX()*leftStick->GetX());
				frontRightMotor->Set(leftStick->GetY()*rightStick->GetX()*-leftStick->GetX());
				backLeftMotor->Set(-leftStick->GetY()*-rightStick->GetX()*leftStick->GetX());
				backRightMotor->Set(-leftStick->GetY()*-rightStick->GetX()*-leftStick->GetX());
			}
		}
	}
};

//START_ROBOT_CLASS(GyroSample);
