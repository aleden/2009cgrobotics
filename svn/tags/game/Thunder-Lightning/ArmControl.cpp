#include "ArmControl.h"
#include "Utils.h"
#include "UserInterface.h"
#include "PIDController.h"
#include <math.h>

//#define PID_TUNING

ArmControl::ArmControl(ChickenRobot* chicken) : Control(chicken), hasJoyYStopped(false),
settingPreset(false)
{
#if 0
	if (getPanelAxis() == 0)
	{
		// Panel not properly connected, SIGNAL ERROR
		disablePanel();
		printf("PANEL NOT CONNECTED\n");
	}
	else
	{
		enablePanel();
	}
	
	setPanelOffset(-(getPanelAxis() + 1));
#else
	setPanelOffset(0);
#endif

	armJaguar = addJaguar(ARM_JAGUAR_PORT, "ARM");
    armJaguarEncoder = addEncoder(ARM_JAGUAR_ENCODER_PORTA, ARM_JAGUAR_ENCODER_PORTB, "ARM");
    clawDoubleActingSolenoidA = addSolenoid(CLAW_DOUBLE_ACTING_SOLENOID_PORTA, "CLAW A");
    clawDoubleActingSolenoidB = addSolenoid(CLAW_DOUBLE_ACTING_SOLENOID_PORTB, "CLAW B");
    elbowDoubleActingSolenoidA = addSolenoid(ELBOW_DOUBLE_ACTING_SOLENOID_PORTA, "ELBOW A");
    elbowDoubleActingSolenoidB = addSolenoid(ELBOW_DOUBLE_ACTING_SOLENOID_PORTB, "ELBOW B");
    armJaguarEncoder->Start();
    armJaguarEncoder->Reset();
	
    armJaguar->SetSafetyEnabled(false);

    pidController = new PIDController(armJaguar, armJaguarEncoder);
    
    setArmPID(0.0f);

    throttle = true;
    
    scriptPosition = 0;
}

void ArmControl::setArmPID(float setPoint)
{ pidController->SetSetpoint(setPoint); }

float ArmControl::getArmPID()
{ return pidController->GetSetpoint(); }

static inline float fbabs(float a)
{ return a < 0.0 ? -a : a; }

static inline double fbabs(double a)
{ return a < 0.0 ? -a : a; }

static inline float Deadzone(float value, float range = .1f)
{ return fbabs(value) <= range ? 0.0f : value; }

void ArmControl::teleopInit()
{
	disablePID();
	armJaguar->Set(0.0f);
}

void ArmControl::teleopPeriodic()
{
	//
	// PID
	//
	setPIDEnabled(getPanelTopSwitch());

	//
	// Claw
	//
	PRESSED_TRIGGER(isClawTrigger, getJoystickTrigger(UserInterface::CO_PILOT_JOYSTICK_PORT) ||
			getJoystickRawButton(UserInterface::PILOT_JOYSTICK_PORT, 5) ||
			getJoystickRawButton(UserInterface::PILOT_JOYSTICK_PORT, 6) ||
			getJoystickRawButton(UserInterface::PILOT_JOYSTICK_PORT, 7) ||
			getJoystickRawButton(UserInterface::PILOT_JOYSTICK_PORT, 8) ||
			getJoystickRawButton(UserInterface::PILOT_JOYSTICK_PORT, 9) ||
			getJoystickRawButton(UserInterface::PILOT_JOYSTICK_PORT, 10));
	if (isClawTrigger)
		setClawOpened(!isClawOpened());

	//
	// Elbow
	//
	PRESSED_TRIGGER(isElbowTrigger, getJoystickRawButton(UserInterface::CO_PILOT_JOYSTICK_PORT, 3));
	if (isElbowTrigger)
		setElbowOpened(!isElbowOpened());

	//
	// Arm move
	//
	float armJoyY = Deadzone(getJoystickY(UserInterface::CO_PILOT_JOYSTICK_PORT));
	if (!isPIDEnabled())
	{
		armJaguar->Set(armJoyY);
	}
	else
	{
		if (armJoyY == 0.0f)
		{
			if (!hasJoyYStopped)
			{
				joyStopPoint = armJaguarEncoder->PIDGet();
				hasJoyYStopped = true;
			}
			setArmPID(joyStopPoint);
		}
		else if (armJoyY > 0.0f)
		{
			setArmPID(150);
			hasJoyYStopped = false;
		}
		else if (armJoyY < 0.0f)
		{
			setArmPID(0);
			hasJoyYStopped = false;
		}
		
		// Make sure  arm isn't too low when elbow is out
		//if (isElbowOpened() && pidController->GetSetpoint() < 18)
		//	setArmPID(18);

		pidController->Update();

		DriverStationLCD::GetInstance()->Printf(DriverStationLCD::kUser_Line1, 1, "pid(%f -> %f)", (float)armJaguarEncoder->PIDGet(), (float)pidController->GetSetpoint());
	}

	//
	// Print PID and panel status
	//
#if 0
	char PIDTrueString[] = "PID: 1";
	char PIDFalseString[] = "PID: 0";
	
	char PanelTrueString[] = "PAN: 1";
	char PanelFalseString[] = "PAN: 0";
	print(isPIDEnabled() ? PIDTrueString : PIDFalseString);
	print(isPanelEnabled() ? PanelTrueString : PanelFalseString);
#endif

	if (!isPIDEnabled() &&
		((getJoystickRawButton(UserInterface::CO_PILOT_JOYSTICK_PORT, 2) && getJoystickRawButton(UserInterface::CO_PILOT_JOYSTICK_PORT, 4)) ||
		 getJoystickRawButton(UserInterface::PILOT_JOYSTICK_PORT, 12)))
		armJaguarEncoder->Reset();
	
	
	//
	// Print
	//
	DriverStationLCD::GetInstance()->Printf(DriverStationLCD::kUser_Line1, 1, "PID:%d, ae:%f", (int)isPIDEnabled(), (float)armJaguarEncoder->PIDGet());
}

void ArmControl::autonomousInit()
{
	//armJaguarEncoder->Reset();
	enablePID();
	openElbow();
	closeClaw();
}

void ArmControl::autonomousPeriodic()
{
	pidController->Update();
	DriverStationLCD::GetInstance()->Printf(DriverStationLCD::kUser_Line1, 1, "pid(%f -> %f)", (float)armJaguarEncoder->PIDGet(), (float)pidController->GetSetpoint());
}

void ArmControl::setAutonomousPreset(bool isIn, bool isLeft, bool isMiddle, bool isOn)
{
	static const char* armPositionStrings[] =
	{
		"Down",
		"Middle peg (low)",
		"Middle peg (high)",
		"Upper peg (low)",
		"Upper peg (high)"
	};

	ARM_PRESET armPosition;

#if 0
	if (isOn)
	{
		if (isIn) // isLow
		{
			if (isMiddle)
			{
				setArmPreset(armPosition = AP_MIDDLE_PEG_LOW);
			}
			else
			{
				setArmPreset(armPosition = AP_MIDDLE_PEG_LOW);
			}
		}
		else
		{
			if (isMiddle)
			{
				setArmPreset(armPosition = AP_UPPER_PEG_HIGH);
			}
			else
			{
				setArmPreset(armPosition = AP_UPPER_PEG_HIGH);
			}
		}
	}
	else
	{
		armPosition = AP_DOWN;
	}
#else
	setArmPreset(armPosition = AP_UPPER_PEG_HIGH);
#endif
	printf("Preset: %s\n", armPositionStrings[armPosition]);
}


void ArmControl::setArmPreset(ARM_PRESET pos)
{
	static const float armPresetSetpoints[] =
	{
		0, // AP_DOWN
		76, // AP_MIDDLE_PEG_LOW
		83, // AP_MIDDLE_PEG_HIGH
		106, // AP_UPPER_PEG_LOW
		112 // AP_UPPER_PEG_HIGH
	};

#if CHICKEN_ROBOT == CHICKEN_LIGHTNING
	static const float lightningArmOffset = 10.0f;

	setArmPID(armPresetSetpoints[pos] + lightningArmOffset);
#elif CHICKEN_ROBOT == CHICKEN_THUNDER
	static const float thunderArmOffset = 6.0f;

	setArmPID(armPresetSetpoints[pos] + thunderArmOffset);
#endif
}

void ArmControl::setClawOpened(bool isOpen)
{
	clawDoubleActingSolenoidA->Set(!isOpen);
	clawDoubleActingSolenoidB->Set(isOpen);
}

void ArmControl::setElbowOpened(bool isOpen)
{
#if CHICKEN_ROBOT == CHICKEN_LIGHTNING
	elbowDoubleActingSolenoidA->Set(!isOpen);
	elbowDoubleActingSolenoidB->Set(isOpen);
#elif CHICKEN_ROBOT == CHICKEN_THUNDER
	elbowDoubleActingSolenoidA->Set(!isOpen);
	elbowDoubleActingSolenoidB->Set(isOpen);
#endif
}

bool ArmControl::isClawOpened()
{
	return clawDoubleActingSolenoidB->Get();
}

bool ArmControl::isElbowOpened()
{
#if CHICKEN_ROBOT == CHICKEN_LIGHTNING
	return elbowDoubleActingSolenoidB->Get();
#elif CHICKEN_ROBOT == CHICKEN_THUNDER
	return elbowDoubleActingSolenoidB->Get();
#endif
}

void ArmControl::lowerArm()
{ setArmPID(getArmPID() - 13); }

void ArmControl::lowerArmSmallAmount()
{ setArmPID(getArmPID() - 10); }

bool ArmControl::isArmLowered()
{ return armJaguarEncoder->Get() <= getArmPID(); }

void ArmControl::setPIDEnabled(bool isEnabled)
{
	if (isEnabled)
		pidController->Enable();
	else
		pidController->Disable();
}

bool ArmControl::isPIDEnabled()
{ return pidController->IsEnabled(); }

