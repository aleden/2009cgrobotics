#include "CatlinPID.h"

CatlinPID::CatlinPID(float kp, float ki, float kd, float setpoint)
{
	p = kp; i = ki; d = kd; this->setpoint = setpoint;
	integral = last_error = 0;
}

void CatlinPID::reset()
{
	integral = last_error = 0;
}

float CatlinPID::calc(float current, float tdelta)
{
	float error = setpoint - current;
	integral += error * tdelta;
	float dir = (error - last_error) / tdelta;
	float retval = error * p + dir * d + integral * i;
	last_error = error;
	return retval;
}
