#include "WPILib.h"
#include "..\RoboticsFramework\RoboticsFramework.h"
using namespace RoboticsFramework;

class MyMotors : public Part
{
	RobotDrive drive;
	Joystick stick;
public:
	MyMotors() : drive(1, 2), stick(1) {}
	~MyMotors() {}
	
	virtual void initOperator()
	{
	}
	virtual void initAutonomous()
	{
	}
	
	virtual void runOperator()
	{
		drive.ArcadeDrive(stick);
		Wait(0.005);
	}
	
	virtual void runAutonomous()
	{
		drive.Drive(0.5, 0.0);
		Wait(2.0);
		drive.Drive(0.0, 0.0); 
	}
};

RUN_FRAMEWORK(new MyMotors());