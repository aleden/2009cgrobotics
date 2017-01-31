#include "SuckControl.h"
#include "hal.h"

SuckControl *sucker = new SuckControl();

void SuckControl::updateSucker()
{
	float tdelta = timer.Get();
	timer.Reset();
	
	float setpoint = 10; //(hal->rStick.GetThrottle() + 1) / 2.0 * 5.0 + 5.0;
	//static int foo = 10;
	//if (foo-- == 0) {
	//	printf("%f\n",setpoint);
	//	foo = 10;
	//}
	const float kP = 0.25; // needs to be tuned
	float realpoint, delta;
	
	switch (state) {
	case suckStop:
		currentVoltage = 0;
		break;
	case suckOut:
		currentVoltage = 1;
		break;
	case suckIn:
		if (currentVoltage > -0.25) currentVoltage = -0.25;
		realpoint = hal->getSuckerCurrent();
		delta = setpoint - realpoint;
		currentVoltage -= delta * kP * tdelta;
	}
	
	if (currentVoltage > 1.0) currentVoltage = 1.0;
	if (currentVoltage < -1.0) currentVoltage = -1.0;
	hal->setSuckerPower(currentVoltage);
}
