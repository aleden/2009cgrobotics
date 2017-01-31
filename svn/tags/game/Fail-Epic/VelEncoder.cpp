#include "WPILib.h"
#include "VelEncoder.h"

const double pi = 3.14159265; // why doesn't math.h define M_PI?
const double minTimeInterval = 0.05; // sample no more than once every 20th of a second

VelEncoder::VelEncoder(float diameter, int clicksPerRotation, int A,int B,bool reverse) :
	encoder(A,B,reverse)
{
	float circumference = diameter * pi;
	float distancePerClick = circumference / clicksPerRotation;
	
	encoder.Start();
	
	encoder.SetDistancePerPulse(distancePerClick);
	
	lastDistance = encoder.GetDistance(); // should be zero
	
	lastVel = 0.0;
	
	timer.Start();
}

float VelEncoder::getVel(bool *isStale)
{
	double tdelta = timer.Get();
	if (tdelta < minTimeInterval) {
		if (isStale != NULL)
			*isStale = true;
		return lastVel;
	}
	if (isStale != NULL)
		*isStale = false;
	
	timer.Reset();
	
	double newDistance = encoder.GetDistance();
	double ddelta = newDistance - lastDistance;
	
	double velocity = ddelta / tdelta;
	
	lastDistance = newDistance;
	lastVel = velocity;
	
	return velocity;
}

double VelEncoder::getDistance()
{
	return encoder.GetDistance();
}

int VelEncoder::getClicks()
{
	return encoder.Get();
}
