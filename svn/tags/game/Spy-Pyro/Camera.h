#pragma once
#include "RoboticsFramework.h"
#include "Target.h"

class Camera : public Part
{
private:
	bool targetAcquired;
	Target target;
	bool hasInitedCam;
	Timer initTimer;
	void setup();
public:
	bool hasTarget() { return targetAcquired; }
	Target getTarget() { return target; }
	float getDistance();
	
	Camera() : Part("camera") { hasInitedCam = false; initTimer.Start(); }

	void initOperator() { initCamera(); }
	void initAutonomous() { initCamera(); }
	void runOperator() {
		checkTimer();
		if (hasInitedCam) updateTarget();
	}
	void runAutonomous() { checkTimer(); if (hasInitedCam) updateTarget(); }
	void runDisabled() { checkTimer(); }
	void runDiagnostics() { checkTimer(); }
	void initCamera();
	void updateTarget();
	
	void checkTimer()
	{
		if (!hasInitedCam && initTimer.Get() > 10) {
#if 1
			setup();
			hasInitedCam = true;
#endif
		}
	}
};

extern Camera* const camera;
