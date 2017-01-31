#pragma once
#include "VelEncoder.h"
#include "WPILib.h"

class AccelEncoder
{
private:
	Timer timer;
	VelEncoder *encoder;
	float lastAccel;
	float lastVel;
	
public:
	AccelEncoder(VelEncoder *vel);
	float getAccel(bool *isStale = NULL);
};
