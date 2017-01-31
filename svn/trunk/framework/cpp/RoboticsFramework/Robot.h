#pragma once
#include "Common.h"
#include "C:\WindRiver\vxworks-6.3\target\h\WPIlib\WPILib.h"

ROBO_FRAMEWORK_BEGIN

// Forward declaration
class Part;

class Robot : public SimpleRobot
{
	static const unsigned int MAX_PARTS = 32;
	Part* parts[MAX_PARTS];
	Part** partsp;
	const char* const name;
public:
	inline Robot(const char* name) : partsp(parts), name(name) {}
	virtual ~Robot() {}

	inline const char* getName() const
	{ return name; }

	inline void addPart(Part* part)
	{ *partsp++ = part; }

	inline Part* getPart(unsigned int index)
	{ return parts[index]; }

	inline unsigned int getNumParts() const
	{ return partsp - parts; }

	virtual bool isRunning() const = 0;
	virtual void initOperator() = 0;
	virtual void initAutonomous() = 0;
	virtual void runOperator() = 0;
	virtual void runAutonomous() = 0;
};

ROBO_FRAMEWORK_END
