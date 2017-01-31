#pragma once
#include "WPILib.h"

class UPanel : public Joystick
{
	double offset;
public:
	UPanel(int port);
    double getAxis();
    bool getLeftButton();
    bool getMiddleButton();
    bool getRightButton();
    bool getTopSwitch();
    bool getLeftSwitch();
    bool getRightSwitch();
    int getKnob();
    void test();
    
    inline void setOffset(double offset)
    { this->offset = offset; }
};
