#pragma once
#include "RoboticsFramework.h"

class Hanger : public Part
{
#if 0
	enum HangerState { hangerGround, hangerUnlatch,
					   hangerDeployed, hangerLatch };
	} state;
	
	static const float latchTime = 2;
#endif
	
	Timer timer;
	bool isServoOpening;
	bool isBarExtended;
public:
	Hanger() : Part("hal"), isServoOpening(false), isBarExtended(false)
	{
	}

	void extendBar();
	void retractWinch(float speed);
	bool hasBarExtended();

	void initOperator();
	void initAutonomous();
	void runOperator();
	void runAutonomous();
};

extern Hanger* const hanger;
