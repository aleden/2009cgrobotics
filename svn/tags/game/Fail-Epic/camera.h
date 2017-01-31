#pragma once
#include <WPILib.h>
#include <TrackAPI.h>
#include "CatlinPID.h"

class CameraRoutines
{
private:
	// Color thresholds (pink and green)
	TrackingThreshold td1, td2;
	ParticleAnalysisReport par1,par2,par;
	
	// Convert distance between colors to distance between robot and target
	// (function determined empirically)
	// Data: 0.3 is 9 ft, 0.5 is 5.5 ft, 0.6 is 4 ft, 0.7 is 3.5 ft
	// Data: 
	template <typename T>
	T convertToRealDistance(const T& distance)
	{
		return 40/(1 + 15*abs(distance));
	}
	
	//CatlinPID pid;
	
public:
	static const unsigned char xDimension = 160;
	static const unsigned char yDimension = 120;
	static const int framesPerSecond   = 15;
	
	CameraRoutines();
	~CameraRoutines();
	
	//void adjustForImage(float xOffset, float distance);
	//void trackColors();
	
	// This is the function which aquires the target
	// Returs true when the target is found, and sets return values
	bool getTarget(float &x, float &y, float &d);
};

//int run_camera(...);

extern CameraRoutines* cameraMan;
