#pragma once
#include "compile config.h"

class AutoTarget
{
public:
	void run();
	
	void enableAutoPan();
	void disableAutoPan();
	void enableAutoFire();
	void disableAutoFire();
	void enableAutoRotate();
	void disableAutoRotate();
	
	float calcShooterSpeed(); // based on lastD, what should the shooter power be
	
	float getTargetAngle(float xpixels);
	float getTargetAngle();
	
	float findRealX();
	float findRealX(float xpixels);
	
	bool isTurretAngleLegit();
	float rotateForTarget();
	bool inRange();
	bool aligned(float x); //aligned for a shot
	
	bool isTargetAquired;
	bool isReadyToFire;
	bool isAutoFiring;
	
	// its 1am
	// fuck accessors
	float turnOffset;
	float panLimit;
	float panCenter;
	
	AutoTarget();
private:
	bool autopan,autofire,autorotate;
	bool panIncreaseP; // is the angle increasing for the pan?
	
	float lastX, lastD, lastY; // coordinates of last x and distance
	
	bool hadImage; // did we just have an image in the last cycle?
	float lastRealX; // last real X offset
	
	Timer imageTimer; // time sence last image was aquired (age of image)
	
	CatlinPID lockonPID; // setpoint is always zero
	
	void run_autopan();
	void run_lockon();
	
	static const float kp = 0.20, ki = 0.0, kd = 0.0; // best values: 0.25, X, 0.0
	static const float maxFiringDistance = 6.5; // trevor adjustment: he's ramming into the fucking trailer
	// 20 deg clockwise, -15 cntr-clockwise
#ifdef FAIL
	static const float maxShootingAnglePos = 20;
	static const float maxShootingAngleNeg = -15;
#endif
#ifdef EPIC
	static const float maxShootingAnglePos = 40;
	static const float maxShootingAngleNeg = -40;
#endif
};

int run_autotarget(...);

extern AutoTarget *autotargeting;
