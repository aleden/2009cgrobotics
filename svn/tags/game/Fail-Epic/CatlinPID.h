#pragma once

class CatlinPID
{
public:
	float p, i, d;
	float setpoint;
	float calc(float current, float tdelta);
	CatlinPID(float kp, float ki, float kd, float setpoint);
	void reset();
private:
	float integral;
	float last_error;
};
