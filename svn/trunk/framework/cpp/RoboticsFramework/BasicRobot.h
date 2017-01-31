#pragma once
#include "Common.h"
#include "Robot.h"
#include <vector>

ROBO_FRAMEWORK_BEGIN

// Forward declaration
class Part;

class BasicRobot : public Robot
{
	const char* name;
	bool running;
public:
	inline BasicRobot(const char* name) : Robot(name), running(true) {}
	inline ~BasicRobot() {}

	inline bool isRunning() const
	{ return running; }

	void initOperator()
	{
		for (unsigned int i = 0; i < getNumParts(); ++i)
		{
			getPart(i)->initOperator();
		}
	}

	void initAutonomous()
	{
		for (unsigned int i = 0; i < getNumParts(); ++i)
		{
			getPart(i)->initAutonomous();
		}
	}

	void runOperator()
	{
		while (isRunning())
		{
			for (unsigned int i = 0; i < getNumParts(); ++i)
			{
				getPart(i)->runOperator();
			}
		}
	}
	void runAutonomous()
	{
		while (isRunning())
		{
			for (unsigned int i = 0; i < getNumParts(); ++i)
			{
				getPart(i)->runAutonomous();
			}
		}
	}
};

ROBO_FRAMEWORK_END
