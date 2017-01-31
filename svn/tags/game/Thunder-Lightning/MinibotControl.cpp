#include "MinibotControl.h"
#include "Utils.h"

MinibotControl::MinibotControl(ChickenRobot* chicken) :
	Control(chicken), retractingCylinder(false), isDeploying(false),
	hasDeployed(false), deploymentStep(0)
{
    deploymentServo = addServo(DEPLOYMENT_SERVO_PORT, "Deploy");
    deploymentSolenoid = addSolenoid(DEPLOYMENT_SOLENOID_PORT, "Deploy");
}

void MinibotControl::teleopInit()
{
    deploymentServo->SetAngle(DEPOLYMENT_SERVO_NOTDEPLOY_ANGLE);
}

void MinibotControl::autonomousInit()
{
    deploymentServo->SetAngle(DEPOLYMENT_SERVO_NOTDEPLOY_ANGLE);
}
void MinibotControl::teleopPeriodic()
{
	PRESSED_TRIGGER(leftSwitchOffTrigger, !getPanelLeftSwitch());
	if (leftSwitchOffTrigger)
	{
		retractingCylinder = true;
		timer.Start();
		timer.Reset();
	}
	
	if (retractingCylinder)
	{
    	deploymentServo->SetAngle(DEPOLYMENT_SERVO_DEPLOY_ANGLE);
    	
    	if (timer.Get() > .5)
    	{
    		retractingCylinder = false;
    	    deploymentSolenoid->Set(false);
    	    
    	    timer.Stop();
    	}
	}
	else
	{
	    deploymentSolenoid->Set(getPanelLeftSwitch());
	    deploymentServo->SetAngle(getPanelRightSwitch() ? DEPOLYMENT_SERVO_DEPLOY_ANGLE : DEPOLYMENT_SERVO_NOTDEPLOY_ANGLE);
	}
}

void MinibotControl::autonomousPeriodic()
{
	deploymentServo->SetAngle(DEPOLYMENT_SERVO_NOTDEPLOY_ANGLE);
}
