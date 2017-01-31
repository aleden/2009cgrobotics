#pragma once
#include "WPILib.h"
#include "Control.h"
#include "Config.h"

class PIDController;

enum ARM_PRESET
{
	AP_DOWN,
	AP_MIDDLE_PEG_LOW,
	AP_MIDDLE_PEG_HIGH,
	AP_UPPER_PEG_LOW,
	AP_UPPER_PEG_HIGH
};

class ArmControl : public Control
{
public:
	PIDController* pidController;
	float p;
    float i;
    float d;
    bool throttle;
    float joyStopPoint;
    bool hasJoyYStopped;

    bool usingPanel;

    int scriptPosition;
    bool scriptEnabled;
    
    bool settingPreset;
    
    Jaguar* armJaguar;
    Solenoid* clawDoubleActingSolenoidA;
    Solenoid* clawDoubleActingSolenoidB;
    Solenoid* elbowDoubleActingSolenoidA;
    Solenoid* elbowDoubleActingSolenoidB;
    
    Encoder* armJaguarEncoder;
    
    static const unsigned int ARM_JAGUAR_PORT = 10;

	static const unsigned int ARM_JAGUAR_ENCODER_PORTA = 5;
	static const unsigned int ARM_JAGUAR_ENCODER_PORTB = 6;

	static const unsigned int CLAW_DOUBLE_ACTING_SOLENOID_PORTA = 1;
	static const unsigned int CLAW_DOUBLE_ACTING_SOLENOID_PORTB = 2;

	static const unsigned int ELBOW_DOUBLE_ACTING_SOLENOID_PORTA = 3;
	static const unsigned int ELBOW_DOUBLE_ACTING_SOLENOID_PORTB = 4;
public:
    ArmControl(ChickenRobot* chicken);
    void teleopPeriodic();
    void autonomousInit();
    void teleopInit();
    void autonomousPeriodic();
    
    void setArmPID(float setPoint);
    float getArmPID();

    void setArmPreset(ARM_PRESET level);
    void setAutonomousPreset(bool inout, bool leftright, bool midhigh, bool onoff);
    
    void setClawOpened(bool isOpen);
    void setElbowOpened(bool isOpen);
    

    bool isClawOpened();
    bool isElbowOpened();

    inline void openClaw()
    { setClawOpened(true); }

    inline void closeClaw()
    { setClawOpened(false); }
    
    inline void openElbow()
    { setElbowOpened(true); }

    inline void closeElbow()
    { setElbowOpened(false); }
    
    inline bool isPanelEnabled()
    { return usingPanel; }
    
    inline void setPanelEnabled(bool usingPanel)
    { this->usingPanel = usingPanel; }

    inline void enablePanel()
    { setPanelEnabled(true); }

    inline void disablePanel()
    { setPanelEnabled(false); }

    void lowerArm();
    void lowerArmSmallAmount();
    bool isArmLowered();
    
    void setPIDEnabled(bool isEnabled);
    
    inline void enablePID()
    { setPIDEnabled(true); }
    
    inline void disablePID()
    { setPIDEnabled(false); }
    
    bool isPIDEnabled();
};
