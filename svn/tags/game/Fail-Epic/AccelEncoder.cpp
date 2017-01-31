#include "WPILib.h"
#include "VelEncoder.h"
#include "AccelEncoder.h"

const double minTimeInterval = 0.05; // sample no more than once every 20th of a second

AccelEncoder::AccelEncoder(VelEncoder *v) :
	encoder(v)
{
	lastVel = encoder->getVel();
	
	lastAccel = 0.0;
	
	timer.Start();
}

float AccelEncoder::getAccel(bool *isStale)
{
	double tdelta = timer.Get();
	if (tdelta < minTimeInterval) {
		if (isStale != NULL)
			*isStale = true;
		return lastAccel;
	}
	if (isStale != NULL)
		*isStale = false;
	
	timer.Reset();
	
	double newVel = encoder->getVel();
	double vdelta = newVel - lastVel;
	
	double accel = vdelta / tdelta;
	
	lastAccel = accel;
	lastVel = newVel;
	
	return accel;
}
