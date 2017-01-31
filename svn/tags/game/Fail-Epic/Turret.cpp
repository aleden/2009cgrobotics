#include "turret.h"
#include "wrappers.h"
#include "compile config.h"

int turretPidUpdate(...);

static CatlinPID pid(0.015, 0.005, 0.0, 0.0); // this is "global" so both the class and the thread callback can get at it

Turret::Turret() :
	enablePIDControl(true),
	turretThread("Turret", turretPidUpdate)
{
	turretThread.Start();
}

Turret::~Turret() { }

void Turret::reset() { pid.reset(); }

void Turret::setToAngle(float angle) { // from -90 to 90 where 0 is center, -90 is right, and 90 is left
	if (angle < -70) angle = -70;
	if (angle > 70) angle = 70;
	pid.setpoint = angle;
}

float Turret::getAngle() {
	return pid.setpoint;
}

int turretPidUpdate(...) {
	Timer timer;
	timer.Start();
	
	for (;;) {
		Wait(0.1);
		
		//pid.p = 0.05 * 0.5 * (hal->getJoystickThrottle(J_1) + 1.0);
		//pid.d = 0.05 * 0.5 * (hal->getJoystickThrottle(J_2) + 1.0);
		//pid.p = 0.05 * (hal->getJoystickThrottle(J_1) + 1.0) * 0.5;
		//pid.p = 0.05 * 0.25;
		
		//cout << "p = " << pid.p << "d = " << pid.d << " i = " << pid.i << endl;
		
		float current = hal->getTurretAngle();
		float delta = pid.calc(current,timer.Get());
		timer.Reset();
		
		// Clamp delta
		
		if(delta > 0.5)
			delta = 0.5;
		else if(delta < -0.5)
			delta = -0.5;
		
		//cout << "enablePIDControl = " << turret->enablePIDControl << endl;
		//cout << "current = " << current << " setpoint = " << pid.setpoint << " delta = " << delta << endl;
#ifndef DUMPER
		if (turret->enablePIDControl)
			hal->setMotorSpeed(M_TURRET, delta);
		else
			turret->reset();
#endif
	}
}
