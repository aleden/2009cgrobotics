#pragma once

#include "RoboticsFramework.h"
#include "WPILib.h"

class SuckControl : public Part
{
private:
	enum SuckState { suckIn, suckOut, suckStop };
	void updateSucker();
	float currentVoltage; // not actually in volts; [-1,1]
	SuckState state;
	Timer timer;
	
public:
	SuckControl() : Part("sucker") {
		currentVoltage = 0;
		state = suckStop;
	}
	void initOperator() { timer.Start(); timer.Reset(); }
	void initAutonomous() { timer.Start(); timer.Reset(); }
	void runOperator() { updateSucker(); }
	void runAutonomous() { updateSucker(); }
	
	float getVoltage() { return currentVoltage; }
	
	void suck() {
		state = suckIn;
		if (currentVoltage >= 0) currentVoltage = -1;
	}
	void blow() { state = suckOut; }
	void stop() { state = suckStop; }
};

extern SuckControl *sucker;
