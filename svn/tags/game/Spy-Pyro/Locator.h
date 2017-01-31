#include "RoboticsFramework.h"
#include "hal.h"

class Locator : public Part
{
private:
	float x,y,theta;
	float pastFL, pastFR, pastBL, pastBR;
	void update();
public:
	Locator() : Part("locator")
	{
		x = y = theta = pastFL = pastFR = pastBL = pastBR = 0;
	}
	
	float getX() { return x; }
	float getY() { return y; }
	float getTheta() { return theta; }
	void reset() {
		x = y = theta = 0;
		pastFL = hal->getFLEncoder();
		pastFR = hal->getFREncoder();
		pastBL = hal->getBLEncoder();
		pastBR = hal->getBREncoder();
	}

	void initOperator() { }
	void initAutonomous() { }
	void runOperator() { update(); }
	void runAutonomous() { update(); }
};

extern Locator* const locator;
