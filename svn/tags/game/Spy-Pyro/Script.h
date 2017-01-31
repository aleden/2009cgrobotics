#pragma once
#include "RoboticsFramework.h"
#include "WPILib.h"

enum scriptID
{bumpID, kickerID, winchID, none};

enum scriptPos
{pos0, pos1, pos1b, pos2, pos3, pos4, pos5, pos6 };

class Script : public Part
{
	//Timer* t;
	float oldTimerValue;
	scriptPos scriptPosition;
	scriptID currentScript;
	float kickWinchThreshold;
	bool kickerScriptRunning;
	
public:
	Script() : Part("script")
	{
		//t = new Timer();
		currentScript = none;
		scriptPosition = pos0;
		kickerScriptRunning = false;
		kickWinchThreshold = 7.0;
	}
	void initOperator();
	void initAutonomous();
	void runOperator();
	void runAutonomous();
	void setKickWinchThreshold(float threshold);
	void runKickScript(); //for use in autonomous
	void abortScript();

private:
	scriptID getCurrentScript();
	scriptPos getScriptPosition();
	void setScriptPosition(scriptPos pos);
	void checkScriptButtons();
	void runScripts(scriptID id);
	
};



extern Script* const script;
