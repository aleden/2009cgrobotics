#pragma once
#include "Common.h"

ROBO_FRAMEWORK_BEGIN

class Part
{
	int type;
	const char* const name;
public:
	Part(const char* name = "", int type = -1) : type(type), name(name) {}
	virtual ~Part() {}

	inline const char* getName() const
	{ return name; }

	inline int getType() const
	{ return type; }

	virtual void initOperator() {}
	virtual void initAutonomous() {}
	virtual void runOperator() {}
	virtual void runAutonomous() {}
};

ROBO_FRAMEWORK_END
