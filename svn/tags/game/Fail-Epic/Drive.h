#pragma once
#include "CatlinPID.h"

class DriveRoutines
{
private:
	Timer *mtrSmoothTimer;
	
	template <typename T>
	T abs(const T& x)
	{ return x < 0 ? -x : x; }
	
	float getGyroCorrectConstant();
	float getSmoothCorrectConstant();
	float gyroCorrect(float joyX, float joyY);
	/////////////////////////////////////////////////////
	// Correction functions
	float pidCorrect(float vel, float velW, float desiredWheel, 
			float currentWheel, double tdelta, CatlinPID &pid,bool doprint);
	float asteroids(float desiredWheel, float currentWheel, float tdelta); // just fucking stupid
	float correctForSlip(float transVel, float omniVel, float desiredWheel, float maxVel, double time);
	float pushMode(float desiredWheel, float omniVel, float maxSpeed, float timeDelta);
	float trevorCorrect(float desiredWheel, float omniVel, bool isRightSide);
public:
	static const float correctP = 0.003;
	static const float correctI = 0.0;
	static const float correctD = 0.0;
	
	DriveRoutines();
	~DriveRoutines();
	void drive(float joyX, float joyY, bool smoothp = true, bool gyrop = true);
	float clamp(float x, float range = 1.0);
};

// driver provides access to using drive routines.
extern DriveRoutines* driver;

