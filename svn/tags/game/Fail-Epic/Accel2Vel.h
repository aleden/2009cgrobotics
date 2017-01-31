#pragma once

class AnalogChannel;

//This class is an accelerometer which integrates over times to get velocity
// Uses hardware integration, just like the gyro :)
class Accel2Vel {
public:
	Accel2Vel(unsigned slot,unsigned channel);
	~Accel2Vel();
	double getVel();
	void reset();
private:
	AnalogChannel *achan; //different from 4chan
	double voltsPerG;
	double zeroGVoltage;
	double offset;
	
	void initAccel(); // sets voltsPerG and zeroGVoltage
};
