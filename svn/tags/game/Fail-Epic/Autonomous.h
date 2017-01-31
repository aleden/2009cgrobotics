#pragma once
#include "WPILib.h"
#include "CatlinPID.h"

enum AUTONOMOUS_MODE
{ AM_ZIGZAG, AM_FOWLER, AM_PROPORTIONAL, AM_SPIN, AM_COLE, AM_POOF, AM_POOF_LEFT, AM_EMPTYCOLLECT, AM_EMPTYCOLLECT_LEFT };

class AutonomousParams
{
public:
	bool top,left,right;
	
	AutonomousParams(){}
	~AutonomousParams(){}
	
	AutonomousParams(const AutonomousParams& ap)
	{ (*this) = ap; }
	
	AutonomousParams(bool t,bool r,bool l)
	{ this->left = l; this->right = r; this->top = t;}
};
class AutonomousRoutines
{
public:
	static const float panSpeed = 0.3; // Best value: 0.3-0.4
	static const float minDistance = 4;    // Distance at which to shoot
	static const float kp = 0.5, ki = 0.5, kd = 0; // Best values: (0.5, 0.5, 0)
	static const float yAdjustSpeed = 0.5; // Best value: 0.5
	static const float dt = 0.05; // Small time to wait
	
	AutonomousRoutines(const AutonomousParams& params);
	~AutonomousRoutines();
	void run_autonomous();
private:
	AUTONOMOUS_MODE mode;
	CatlinPID pid;
	static const int escapeTime = 100; // how long robot escapes for amFowler
	
	void amCole();
	void amZigzag();
	void amFowler();
	void amProportional();
	void amSpin();
	void robotTrack(float &previousX);
	float determineXSpeedFromAngle(float angle);// - to right of robot, + to right of robot, angle in degrees
	void amPoof(bool turnIn = true);
	void amPoofLeft(bool turnIn = true); // turn left
	
	void amEmptyCollect();
	void amEmptyCollectLeft();
	
	void setMode(const AutonomousParams& params);
};

extern AutonomousRoutines *autonomous;
