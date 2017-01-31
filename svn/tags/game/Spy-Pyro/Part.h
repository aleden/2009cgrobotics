#pragma once
#include "Common.h"

class Part
{
	unsigned int id;
	const char* const name;
public:
	Part(const char* const name = "", int id = -1) : id(id), name(name) {}
	virtual ~Part() {}

	inline const char* getName() const
	{ return name; }

	inline int getID() const
	{ return id; }

	virtual void initOperator() {}
	virtual void initAutonomous() {}
	virtual void runOperator() {}
	virtual void runAutonomous() {}
	virtual void runDisabled() {}
	virtual void runDiagnostics() {}
};
