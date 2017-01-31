#include "Hanger.h"
#include "hal.h"

Hanger* const hanger = new Hanger();

void Hanger::initOperator()
{
}

void Hanger::initAutonomous()
{
}

void Hanger::runOperator()
{
	if (isServoOpening)
	{
		if (timer.Get() > 1.0)
		{
			hal->setArm(true);
			isBarExtended = true;
			isServoOpening = false;
		}
	}
}

void Hanger::runAutonomous()
{
}

void Hanger::extendBar()
{
	hal->setArmServoAngle(hal->latchUnlockedAngle); // Unlock
	isServoOpening = true;
	timer.Start();
	timer.Reset();
}

bool Hanger::hasBarExtended()
{
	return isBarExtended;
}

void Hanger::retractWinch(float speed)
{
	hal->setWinchHangPower(speed);
	//added by rohan for diagnostic purposes
	//float fpCurrent = hal-> getWinchFPCurrent();
	//float cimCurrent = hal-> getWinchCIMCurrent();
	//printf("FP Current: %f, CIM Curent: %f\n", fpCurrent, cimCurrent);
}
