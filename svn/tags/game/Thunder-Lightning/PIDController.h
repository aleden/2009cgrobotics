#ifndef PIDCONTROLLER_H_
#define PIDCONTROLLER_H_

#include "Base.h"
#include "semLib.h"
#include "PIDSource.h"
#include "Config.h"

class PIDOutput;

// WPIHack PIDController (no HW interrupts blocking kernel)
class PIDController
{
private:
	PIDSource* source;
	PIDOutput* output;
	float setpoint;
	bool isEnabled;

#if CHICKEN_ROBOT == CHICKEN_LIGHTNING
	static const float speed = 1.0f;
	static const float lowSpeed = .7f;
	static const float veryLowSpeed = .3f;
#elif CHICKEN_ROBOT == CHICKEN_THUNDER
	static const float speed = 1.0f;
	static const float lowSpeed = .8f;
	static const float veryLowSpeed = .3f;
#endif
public:
	
	/**
	 * Allocate a PID object with the given constants for P, I, D
	 * @param Kp the proportional coefficient
	 * @param Ki the integral coefficient
	 * @param Kd the derivative coefficient
	 * @param source The PIDSource object that is used to get values
	 * @param output The PIDOutput object that is set to the output value
	 */
	inline PIDController(PIDOutput* output, PIDSource* source) :
		source(source), output(output), setpoint(0.0f), isEnabled(false) {}

	inline float GetSetpoint()
	{ return setpoint; }

	inline void SetSetpoint(float setpoint)
	{ this->setpoint = setpoint; }
	
	inline bool IsEnabled()
	{ return isEnabled; }

	inline void SetEnabled(bool isEnabled)
	{ this->isEnabled = isEnabled; }

	inline void Enable()
	{ SetEnabled(true); }
	
	inline void Disable()
	{ SetEnabled(false); }
	
	inline bool HasReachedSetpoint()
	{ return source->PIDGet() == GetSetpoint(); }

	void Update();
};

#endif
