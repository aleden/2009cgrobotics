#pragma once
#include "WPILib.h"
#include "ChickenRobot.h"

enum PANEL_BUTTON
{
	PANEL_BUTTON_CAM0,
	PANEL_BUTTON_CAM1,
	PANEL_BUTTON_CAM2,
};

// Forward declaration
class ChickenRobot;

class Control
{
    ChickenRobot* chicken;

public:
    inline Control(ChickenRobot* chicken) : chicken(chicken) {}
    virtual ~Control() {}

    inline CANJaguar* addCANJaguar(int port, const char* name)
    { return chicken->addCANJaguar(port, name); }

    inline Victor* addVictor(int port, const char* name)
    { return chicken->addVictor(port, name); }

    inline Jaguar* addJaguar(int port, const char* name)
    { return chicken->addJaguar(port, name); }

    inline Encoder* addEncoder(int port1, int port2, const char* name)
    { return chicken->addEncoder(port1, port2, name); }

    inline Solenoid* addSolenoid(int port, const char* name)
    { return chicken->addSolenoid(port, name); }
    
    inline Servo* addServo(int port, const char* name)
    { return chicken->addServo(port, name); }

    inline DigitalInput* addDigitalInput(int port, const char* name)
    { return chicken->addDigitalInput(port, name); }
    
    inline AnalogChannel* addAnalogChannel(int port, const char* name)
    { return chicken->addAnalogChannel(port, name); }

    inline double getJoystickX(int port)
    { return chicken->getJoystickX(port); }

    inline bool getJoystickRawButton(int port, int button)
    { return chicken->getJoystickRawButton(port,button); }

    inline double getJoystickY(int port)
    { return chicken->getJoystickY(port); }

    inline bool getJoystickTrigger(int port)
    { return chicken->getJoystickTrigger(port); }

    inline double getJoystickThrottle(int port)
    { return chicken->getJoystickThrottle(port); }

    inline void setPanelOffset(double offset)
    { chicken->setPanelOffset(offset); }

    inline double getPanelAxis()
    { return chicken->getPanelAxis(); }

    inline bool getPanelLeftButton()
    { return chicken->getPanelLeftButton(); }

    inline bool getPanelMiddleButton()
    { return chicken->getPanelMiddleButton(); }

    inline bool getPanelRightButton()
    { return chicken->getPanelRightButton(); }

    inline bool getPanelTopSwitch()
    { return chicken->getPanelTopSwitch(); }

    inline bool getPanelLeftSwitch()
    { return chicken->getPanelLeftSwitch(); }

    inline bool getPanelRightSwitch()
    { return chicken->getPanelRightSwitch(); }

    inline int getPanelKnob()
    { return chicken->getPanelKnob(); }

    inline double getMatchTimer()
    { return chicken->getMatchTimer(); }

    inline ChickenDashboardServer* getDashboard()
    { return chicken->getDashboard(); }
    
    virtual void teleopPeriodic() = 0;
    virtual void autonomousPeriodic() = 0;
    inline virtual void teleopInit() {}
    inline virtual void autonomousInit() {}
};

