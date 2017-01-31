#include "LEDControl.h"

LEDControl::LEDControl(ChickenRobot* chicken) :
	Control(chicken),
    cameraOn(false),
#if CHICKEN_ROBOT == CHICKEN_LIGHTNING
    whiteOn(false),
    blueOn(false),
    redOn(false),
	toggle(false),
	blueWhite(new Relay(BLUE_WHITE_LED_PORT)),
	redCamera(new Relay(RED_CAMERA_LED_PORT))
#else
	cameraRelay(new Relay(CAMERA_LED_PORT))
#endif
{
}

void LEDControl::autonomousInit()
{
#if CHICKEN_ROBOT == CHICKEN_LIGHTNING
#ifdef FLICKER
	timer.Stop();
	timer.Reset();
	toggle = false;
#endif
#endif
}
void LEDControl::teleopInit()
{
#if CHICKEN_ROBOT == CHICKEN_LIGHTNING
#ifdef FLICKER
	timer.Start();
	timer.Reset();
	toggle = false;
#endif
#endif
}

void LEDControl::autonomousPeriodic()
{
	setLEDs();
}

void LEDControl::teleopPeriodic()
{
#if CHICKEN_ROBOT == CHICKEN_LIGHTNING
	int knobSetting = getPanelKnob();
	switch (knobSetting)
	{
		case 0:
			blueOn = false;
		    whiteOn = false;
		    redOn = false;
		    break;
		case 1:
			blueOn = false;
		    whiteOn = false;
		    redOn = true;
		    break;
		case 2:
			blueOn = false;
		    whiteOn = true;
		    redOn = false;
		    break;
		case 3:
			blueOn = true;
		    whiteOn = false;
		    redOn = false;
		    break;
		default:
			blueOn = false;
		    whiteOn = false;
		    redOn = false;
		    break;
	}
	
#endif
	setLEDs();
}

void LEDControl::setCameraLEDs(bool on)
{
	cameraOn = on;
}

void LEDControl::setLEDs()
{
#if CHICKEN_ROBOT == CHICKEN_LIGHTNING
	if (redOn && cameraOn)
		redCamera->SetDirection(Relay::kReverseOnly);
	else
		redCamera->SetDirection(Relay::kBothDirections);
#ifdef FLICKER
	if (timer.Get() > .2)
	{
		toggle = !toggle;
		timer.Reset();
	}
#endif

#ifdef FLICKER
	if (!toggle)
	{
#endif
		if (blueOn)
		{
			if (whiteOn)
			{
				blueWhite->Set(Relay::kOn);
			}
			else
			{
				blueWhite->Set(Relay::kForward);
			}
		}
		else
		{
			if (whiteOn)
			{
				blueWhite->Set(Relay::kReverse);
			}
			else
			{
				blueWhite->Set(Relay::kOff);
			}
		}
		if (cameraOn)
		{
			if (redOn)
			{
				redCamera->Set(Relay::kOn);
			}
			else
			{
				redCamera->Set(Relay::kForward);
			}
		}
		else
		{
			if (redOn)
			{
				redCamera->Set(Relay::kReverse);
			}
			else
			{
				redCamera->Set(Relay::kOff);
			}
		}
#ifdef FLICKER
	}
	else
	{
		blueWhite->Set(Relay::kOff);
		redCamera->Set(Relay::kOff);
	}
#endif

#else
	cameraRelay->Set(cameraOn ? Relay::kReverse : Relay::kForward);
#endif
}
