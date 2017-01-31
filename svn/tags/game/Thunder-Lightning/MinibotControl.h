#pragma once
#include "WPILib.h"
#include "Control.h"

class MinibotControl : public Control
{
private:
	bool retractingCylinder;
    bool isDeploying;
    bool hasDeployed;
    int deploymentStep;
    
    static const unsigned int DEPLOYMENT_SERVO_PORT = 1;
    static const unsigned int DEPLOYMENT_SOLENOID_PORT = 6;
    
    static const float DEPOLYMENT_SERVO_DEPLOY_ANGLE = 107;
    static const float DEPOLYMENT_SERVO_NOTDEPLOY_ANGLE = 180;
    
    Servo* deploymentServo;
    Solenoid* deploymentSolenoid;
    
    Timer timer;
    
public:
    MinibotControl(ChickenRobot* chicken);
    void deploy();
    
    void teleopInit();
    void autonomousInit();
    void teleopPeriodic();
    void autonomousPeriodic();
};
