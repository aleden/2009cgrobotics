#include "DiagnosticsTrigger.h"
#include "ChickenRobot.h"

int counter = 0;

DiagnosticsTrigger::DiagnosticsTrigger(ChickenRobot* chicken)
{
    int i = 0;
    if (chicken->isJoystick(1))
        joystickPortsToRealJoystickPorts[i++] = 1;
    if (chicken->isJoystick(2))
        joystickPortsToRealJoystickPorts[i++] = 2;
    if (chicken->isJoystick(3))
        joystickPortsToRealJoystickPorts[i++] = 3;

    while (i < 3)
    {
        joystickPortsToRealJoystickPorts[i] = -1;
        ++i;
    }

    this->chicken = chicken;
        
    button = counter % NUM_BUTTONS + START_BUTTON;

    joystickPort = joystickPortsToRealJoystickPorts[counter / NUM_BUTTONS];
    doWeHaveATriggerForThis = joystickPort > 0;
        
    ++counter;
}

bool DiagnosticsTrigger::isOn()
{ return doWeHaveATriggerForThis && chicken->getJoystickRawButton(joystickPort, button); }
