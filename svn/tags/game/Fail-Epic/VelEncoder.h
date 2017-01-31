#pragma once

#include "WPILib.h"

class VelEncoder
{
private:
	Timer timer;
	Encoder encoder;
	float lastDistance;
	float lastVel;
	
public:
	VelEncoder(float diameter, int clicksPerRotation, int A,int B,bool reverse=false);
	float getVel(bool *isStale = NULL);
	double getDistance();
	int getClicks();
};
