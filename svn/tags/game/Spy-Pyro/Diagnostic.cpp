#include "Diagnostic.h"
#include "hal.h"
#include "WPILIb.h"
#include "locator.h"
#include <math.h>

Diagnostics* const diagnostics = new Diagnostics();

/*static float deadzone(float x, float radius)
{
	if (fabs(x) < radius) return 0.0;
	return x;
}*/


void Diagnostics::runDiagnostics()
{
#if 1
	static int tmr = 0;
	if (tmr++ > 1000)
	{
		tmr = 0;
		//printf("Diagnostics::runDiagnostics()\n");
	}
	
	hal->stopMotors();

	locator->reset();

	if (hal->rStick.GetRawButton(3))
		hal->setSuckerPower(hal->rStick.GetY());

	hal->setKickerLatch(hal->rStick.GetTrigger());

	hal->setWinchShootPower(0.f);
	
	if (hal->lStick.GetRawButton(3))
		hal->setWinchShootPower(hal->lStick.GetY());

	hal->setArm(hal->lStick.GetTrigger());

	if (hal->hStick.GetRawButton(3)) {
		float y = hal->hStick.GetY();
		hal->setWinchHangPower(-y);
		//printf("setting hanger to %f\n",y);
	}
	
	hal->setKickerForwardValue(hal->hStick.GetTrigger());
	
	if (hal->rStick.GetRawButton(2))
		hal->setArmServoAngle(hal->latchUnlockedAngle); // Unlock
	if (hal->lStick.GetRawButton(2))
		hal->setArmServoAngle(hal->latchLockedAngle); // Lock

	if (hal->hStick.GetRawButton(6))
		hal->setFLPower(hal->hStick.GetThrottle());
	if (hal->hStick.GetRawButton(7))
		hal->setBLPower(hal->hStick.GetThrottle());
	if (hal->hStick.GetRawButton(11))
		hal->setFRPower(hal->hStick.GetThrottle());
	if (hal->hStick.GetRawButton(10))
		hal->setBRPower(hal->hStick.GetThrottle());
#endif
}
