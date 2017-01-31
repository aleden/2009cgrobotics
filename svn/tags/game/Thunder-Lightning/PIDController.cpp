/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "PIDController.h"
#include "Notifier.h"
#include "PIDSource.h"
#include "PIDOutput.h"
#include <math.h>
#include "Synchronized.h"

static inline float Deadzone(float value, float range = .15f)
{ return fabs(value) < range ? 0.0f : value; }

template <typename T>
static inline T Min(T a, T b)
{ return a < b ? a : b; }

void PIDController::Update()
{
	if (IsEnabled())
	{
		printf("Set Point: %f", GetSetpoint());
		//postive UP negative DOWN
		float sourcePoint = source->PIDGet();
		float difference = Deadzone(GetSetpoint() - sourcePoint, 1.0f);
		if (difference != 0.0f)
		{
			float differenceSign = difference < 0.0 ? -1.0f : 1.0f;
			float goalSpeed;
			
			if (fabs(difference) < 3.0f)
				goalSpeed = differenceSign*veryLowSpeed;
			else if (fabs(difference) < 20.0f)
				goalSpeed = differenceSign*lowSpeed;
			else
				goalSpeed = differenceSign*speed;
	
			if (difference != 0.0f)
				output->PIDWrite(goalSpeed);
			else
				output->PIDWrite(0.0f);
		}
	}
}
