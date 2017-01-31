#include "Accel2Vel.h"
#include "AnalogChannel.h"
#include "AnalogModule.h"
#include "Utility.h"
#include "WPIStatus.h"
#include "Timer.h"

//stolen from gyro.cpp
static const UINT32 kOversampleBits = 7;
static const UINT32 kAverageBits = 3;
static const float kCalibrationSampleTime = 5.0;
static const float kSamplesPerSecond = 50.0;

Accel2Vel::Accel2Vel(unsigned slot,unsigned channel)
{
	achan = new AnalogChannel(slot,channel);
	initAccel();
}

void Accel2Vel::initAccel()
{
	if (!achan->IsAccumulatorChannel()) { // we need accumulator for integration
		printf ("ERROR: accel chan not accumulator\n");
		delete achan;
		achan = NULL;
		return;
	}
	
	voltsPerG = 1.0;
	zeroGVoltage = 2.5;
	
	// mostly stolen from Gyro.cpp
	achan->SetAverageBits(kAverageBits);
	achan->SetOversampleBits(kOversampleBits);
	float sampleRate = kSamplesPerSecond * (1 << (kAverageBits + kOversampleBits));
	achan->GetModule()->SetSampleRate(sampleRate);
	Wait(1.0); // calibration
	
	achan->InitAccumulator();
	Wait(kCalibrationSampleTime);
	
	INT64 value;
	UINT32 count;
	
	achan->GetAccumulatorOutput(&value, &count);
	
	UINT32 center = (UINT32)((float)value / (float)count + .5);
	
	offset = ((float)value / (float)count) - (float)center;
	
	achan->SetAccumulatorCenter(center);
	achan->SetAccumulatorDeadband(0);
	achan->ResetAccumulator();
}

void Accel2Vel::reset()
{
	achan->ResetAccumulator();
}

// get the current velocity
double Accel2Vel::getVel()
{
	if (achan == NULL)
		return 0.0;
	
	// "adapted" (stolen) from gyro.cpp
	INT64 rawValue;
	UINT32 count;
	
	achan->GetAccumulatorOutput(&rawValue, &count);
	
	INT64 value = rawValue - (INT64)((float)count * offset);
	
	double scaledValue = value * 1e-9 * (double)achan->GetLSBWeight() * (double)(1 << achan->GetAverageBits()) /
	                       (achan->GetModule()->GetSampleRate() * voltsPerG);
	
	return scaledValue;
}


Accel2Vel::~Accel2Vel()
{
	if (achan != NULL)
		delete achan;
}
