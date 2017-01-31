#include "UPanel.h"

UPanel::UPanel(int port) :
	Joystick(port), offset(0.0)
{
}

double UPanel::getAxis() { return GetX() + offset; }
bool UPanel::getLeftButton() { return GetRawButton(4); }
bool UPanel::getMiddleButton() { return GetRawButton(5); }
bool UPanel::getRightButton() { return GetRawButton(3); }
bool UPanel::getTopSwitch() { return GetRawButton(6); }
bool UPanel::getLeftSwitch() { return GetRawButton(1); }
bool UPanel::getRightSwitch() { return GetRawButton(2); }
int UPanel::getKnob()
{
	if (GetRawButton(9))
	{
		return 1;
	}
	else if (GetRawButton(8))
	{
		return 2;
	}
	else if (GetRawButton(7))
	{
		return 3;
	}
	else
	{
		return 0;
	}
}
void UPanel::test()
{ 
	if (getLeftButton()) printf("fucking shit");
	else printf("fuck");
}
