#pragma once
#include "WPILib.h"
#include "Control.h"
#include "Config.h"

class LEDControl : public Control
{
private:
    bool cameraOn;

#if CHICKEN_ROBOT == CHICKEN_LIGHTNING
    bool whiteOn;
    bool blueOn;
    bool redOn;
    bool toggle;
	
    static const unsigned int BLUE_WHITE_LED_PORT = 2;
    static const unsigned int RED_CAMERA_LED_PORT = 3;
    
    Relay* blueWhite;
    Relay* redCamera;
#ifdef FLICKER
    Timer timer;
#endif
#else
    static const unsigned int CAMERA_LED_PORT = 3;
    
    Relay* cameraRelay;
#endif

public:
    LEDControl(ChickenRobot* chicken);
    void teleopPeriodic();
    void setLEDs();
    void setCameraLEDs(bool on);
    void autonomousPeriodic();
    void autonomousInit();
    void teleopInit();
};
