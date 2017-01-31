#include "RoboticsFramework.h"

class Control : public Part
{
public:
	enum automode { mode0, mode1, mode2 };
private:
	float deadzone(float x, float radius = 0.15);
	
	automode mode;
	
	void runScoreAuto();
	void runMidAuto();
	void runDefAuto();
public:	
	Control() : Part("control"), mode(mode0)
	{
	}

	void initOperator() {}
	void initAutonomous();
	void runOperator();
	void runAutonomous();
	void runDisabled();
	void runDiagnostics();
	automode getAutoMode() { return mode; }
};

extern Control* const control;
