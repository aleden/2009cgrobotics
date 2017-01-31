#pragma once
#include "WPILib.h"
#include "CatlinPID.h"

class Turret {
public:
	static const float	maxVolts			  = 5.275f; // apporximate, assuming linear scale and 2.7V = center
	static const float	minVolts			  = .125f; // 90d right
	static const float	degreesPerVolt		  = 34.9515f; //180.f/(pMaxVolts - pMinVolts);
	
	bool enablePIDControl; // should the turret object control the turret?
	
	Turret();
	~Turret();
	void setToAngle(float angle); // from -90 to 90 where 0 is center, -90 is right, and 90 is left
	float getAngle();
	
	void reset();
	
private:
	Task turretThread;
	//CatlinPID pid;
	//float angleToVolts(float angle);
	//float voltsToAngle(float volts);
};

extern Turret *turret;
