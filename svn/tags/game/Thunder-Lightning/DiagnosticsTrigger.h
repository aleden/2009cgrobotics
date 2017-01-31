#pragma once
#include "WPILib.h"

// Forward declaration
class ChickenRobot;

class DiagnosticsTrigger
{
public:
    static const int START_BUTTON = 2;
    static const int END_BUTTON = 11;
    static const int NUM_BUTTONS = END_BUTTON - START_BUTTON + 1;

    int joystickPort;
    int button;
    ChickenRobot* chicken;
    int joystickPortsToRealJoystickPorts[3];
    bool doWeHaveATriggerForThis;

    DiagnosticsTrigger(ChickenRobot* chicken);
    bool isOn();

    inline int getJoystickPort()
    { return joystickPort; }
};
