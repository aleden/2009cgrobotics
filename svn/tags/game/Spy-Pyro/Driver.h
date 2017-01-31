#include "RoboticsFramework.h"

class Driver : public Part
{
public:
	// mecanum tuning constants
	// <FRONT/BACK><RIGHT/LEFT><POSITIVE/NEGATIVE>
	// this was on blocks
#ifdef SPY
	static const float frP=1.0, frN=0.987578,
					   flP=0.878453, flN=0.901701,
					   brP=0.791045, brN=0.780678,
					   blP=0.853309, blN=0.825260;
#else
	static const float frP = 0.976161,
	frN = 0.976641,
	flP = 0.927171,
	flN = 0.888988,
	brP = 1.000000,
	brN = 0.980499,
	blP = 0.997739,
	blN = 0.967600;
#endif
	// this was on carpet, which, btw, SUCKS ASS for tuning purposes
	/*static const float frP=1.0, frN=0.98,
						   flP=0.95, flN=0.96,
						   brP=0.94, brN=.94,
						   blP=0.96, blN=0.96;*/
private:
	float x,y,r;
	void updateDriver();
	void updateTune();
public:
	bool enableGyroCorrect;
	Driver() : Part("driver")
	{
		x = y = r = 0.0;
	}

	// this is stupid.
	// ALE - I agree 
	void drive(float x,float y,float r) {
		//printf("driver: %f %f %f\n",x,y,r);
		this->x = x; this->y = y; this->r = r; }
	void setX(float x) { this->x = x; }
	void setY(float y) { this->y = y; }
	void setR(float r) { this->r = r; }
	float getX() { return x; }
	float getY() { return y; }
	float getR() { return r; }

	void initOperator() { x = y = r = 0.0f; }
	void initAutonomous() { x = y = r = 0.0f; }
	void runOperator() { updateDriver(); }
	void runAutonomous() { updateDriver(); }
};

extern Driver* const driver;
