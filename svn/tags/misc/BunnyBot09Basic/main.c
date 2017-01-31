#define _FRC_BOARD

#include "BuiltIns.h"
#include "API.h"
#include "types.h"


////////////////////////////////////////////////////////
// Configuration
//#define HENRY_ROBOT
#define ROHISHA_ROBOT

#define TURBO_MODE
#define DEBUG_PRINT
//#define DEBUG_PRINT_DRIVE

////////////////////////////////////////////////////////
// Misc
#define BOOL unsigned char
#define TRUE 1
#define FALSE 0

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) > (b) ? (b) : (a))
#define ABS(a) ((a) < 0 ? (-(a)) : (a))

#define BETWEEN(NUM, START, END) ((NUM) >= START && (NUM) <= END)
#define TOGGLE_BOOL(NUM) ((NUM) = ((NUM) ? 0 : 1))

////////////////////////////////////////////////////////
// Ports
#define MTR_LDRIVE_PORT 1
#define MTR_RDRIVE_PORT 2
#define MTR_ARM_PORT 3

#ifdef ROHISHA_ROBOT
#define MTR_WRIST_PORT 4
#endif

#define RELAY_COMPRESSOR_PORT 1
#define RELAY_GRIPPER_PORT 2
#define RELAY_WRIST_PORT 3

#define DIO_COMPRESSOR_SENSOR_PORT 1
#define DIO_ARM_TOP_LIMIT_SWITCH_PORT 2
#define DIO_ARM_BOTTOM_LIMIT_SWITCH_PORT 3

#ifdef ROHISHA_ROBOT
#define DIO_WRIST_BOTTOM_LIMIT_SWITCH_PORT 4
#endif

#define JOYSTICK_DRIVE_PORT 1
#define JOYSTICK_ARM_PORT 2

////////////////////////////////////////////////////////
// Drive system

#define MTR_SPEED_STOP 127
#define MTR_LDRIVE_SPEED_STOP MTR_SPEED_STOP
#define MTR_RDRIVE_SPEED_STOP MTR_SPEED_STOP

#define MTR_LDRIVE_SPEED_FORWARD  0

#ifdef ROHISHA_ROBOT
#define MTR_LDRIVE_REVERSED
#endif

#define MTR_LDRIVE_SPEED_BACK 255

#define MTR_RDRIVE_SPEED_FORWARD 255

#define MTR_RDRIVE_SPEED_BACK 0

#define REVERSE_MOTOR_SPEED(MTRSPEED) (MTRSPEED) = (255 - (MTRSPEED))

#define DEADRANGE 20
#define DEADZONE(num) (((num) >= (127-DEADRANGE) && (num) <= (127+DEADRANGE)) ? 127 : (num))

void OperatorControl(void)
{
#ifdef DEBUG_PRINT	
	UInt c;
#endif

	//////////////////////////////////////////////////////////////////
	// Drive Control
	{
#ifdef TURBO_MODE
		BOOL turboMode = GetOIDInput(JOYSTICK_DRIVE_PORT, TRIGGER_SW);
#endif

		Int mtrLSpd = MTR_SPEED_STOP;
		Int mtrRSpd = MTR_SPEED_STOP;

		Int joyX = DEADZONE(GetOIAInput(JOYSTICK_DRIVE_PORT, X_AXIS));
		Int joyY = DEADZONE(GetOIAInput(JOYSTICK_DRIVE_PORT, Y_AXIS));

		Int vJoyX = joyX - 127;
		Int vJoyY = joyY - 127;

#ifdef HENRY_ROBOT
		vJoyX = -vJoyX;
#endif

#ifdef TURBO_MODE
		if (!turboMode)
		{
			vJoyY /= 3;
			vJoyX /= 2;
		}
#endif

		mtrLSpd += vJoyY;
		mtrRSpd += vJoyY;

		mtrLSpd += vJoyX;
		mtrRSpd -= vJoyX;

#ifdef DEBUG_PRINT_DRIVE
		if (c < 999*3)
		{
			++c;
		}
		else
		{
#ifdef TURBO_MODE
			printf(turboMode != 0 ? "[%d, %d] (%d, %d) | turboModeON\r" :
				"[%d, %d] (%d, %d) | turboModeOFF\r",
				mtrLSpd, mtrRSpd, joyX, joyY), c = 0;
#else
			printf("(%d, %d)\r", joyX, joyY), c = 0;
#endif
		}
#endif

		mtrLSpd = MIN(255, MAX(mtrLSpd, 0));
		mtrRSpd = MIN(255, MAX(mtrRSpd, 0));

#ifdef MTR_LDRIVE_REVERSED
		REVERSE_MOTOR_SPEED(mtrLSpd);
#endif

#ifdef MTR_RDRIVE_REVERSED
		REVERSE_MOTOR_SPEED(mtrRSpd);
#endif

		SetPWM(MTR_LDRIVE_PORT,  (unsigned char)mtrLSpd);
		SetPWM(MTR_RDRIVE_PORT, (unsigned char)mtrRSpd);
	}

	//////////////////////////////////////////////////////////////////
	// Arm & Gripper control
	{
#ifdef HENRY_ROBOT
		static BOOL gripperToggle = FALSE;
		static BOOL lastGripperToggle = FALSE;

		static BOOL wristToggle = FALSE;
		static BOOL lastWristToggle = FALSE;

		unsigned char armMove = DEADZONE(GetOIAInput(JOYSTICK_ARM_PORT, Y_AXIS));

		BOOL newGripperToggle = GetOIDInput(JOYSTICK_ARM_PORT, TOP_SW);
		BOOL newWristToggle = GetOIDInput(JOYSTICK_ARM_PORT, TRIGGER_SW);

		BOOL armLimitSwitchTop = !GetDigitalInput(DIO_ARM_TOP_LIMIT_SWITCH_PORT);
		BOOL armLimitSwitchBottom = !GetDigitalInput(DIO_ARM_BOTTOM_LIMIT_SWITCH_PORT);
		
		if (!(armLimitSwitchTop && armMove < 127) && !(armLimitSwitchBottom && armMove > 127))
			SetPWM(MTR_ARM_PORT, armMove);
		else
			SetPWM(MTR_ARM_PORT, 127);

		if (newGripperToggle == 1 && lastGripperToggle == 0)
			TOGGLE_BOOL(gripperToggle);

		if (newWristToggle == 1 && lastWristToggle == 0)
			TOGGLE_BOOL(wristToggle);

		SetRelay(2, wristToggle, 0);
		SetRelay(8, gripperToggle, 0);

		lastGripperToggle = newGripperToggle;
		lastWristToggle = newWristToggle;

#endif // HENRY_ROBOT

#ifdef ROHISHA_ROBOT
		// Arm joystick controls height of wrist when top button is not pressed;
		// otherwise, it controls the inclination of the wrist
		static BOOL toggleGripper = FALSE;
		static BOOL lastToggleGripper = FALSE;
		BOOL inclineWrist = GetOIDInput(JOYSTICK_ARM_PORT, TOP_SW);
		BOOL newToggleGripper = GetOIDInput(JOYSTICK_ARM_PORT, TRIGGER_SW);
		unsigned char armMove = DEADZONE(GetOIAInput(JOYSTICK_ARM_PORT, Y_AXIS));

		BOOL armLimitSwitchTop = !GetDigitalInput(DIO_ARM_TOP_LIMIT_SWITCH_PORT);
		BOOL armLimitSwitchBottom = !GetDigitalInput(DIO_ARM_BOTTOM_LIMIT_SWITCH_PORT);
		BOOL wristLimitSwitchBottom = !GetDigitalInput(DIO_WRIST_BOTTOM_LIMIT_SWITCH_PORT);

        if (!inclineWrist)
		{
			if (!(armLimitSwitchTop && armMove > 127) && !(armLimitSwitchBottom && armMove < 127))
				SetPWM(MTR_ARM_PORT, armMove);
			else
				SetPWM(MTR_ARM_PORT, 127);

			SetPWM(MTR_WRIST_PORT, 127);
		}
		else
		{
			SetPWM(MTR_WRIST_PORT, armMove);
			//if (!(wristLimitSwitchBottom && armMove < 127))
				//SetPWM(MTR_WRIST_PORT, armMove);
			//else
				//SetPWM(MTR_WRIST_PORT, 127);
		}

		if (newToggleGripper == 1 && lastToggleGripper == 0)
			TOGGLE_BOOL(toggleGripper);

		SetRelay(2, FALSE, toggleGripper);

		lastToggleGripper = newToggleGripper;
#endif // ROHISHA_ROBOT
	}
	
}

// It is automatically referenced by WPILib at startup and run.
void IO_Initialization(void)
{
	// Basically, a mode of 0 is the default (without the
	// IO_Initialization function) and runs a main function only.
	// SetCompetitionMode(1) runs a competition project
#ifdef DEBUG_PRINT
	printf("IO_Initialization()\r");
#endif
	SetCompetitionMode(1);
}

void main(void)
{
#ifdef DEBUG_PRINT
	printf("main()\r");
#endif
}

// Initialize is run immediately when the robot is powered on regardless
// of the field mode
void Initialize(void)
{
#ifdef DEBUG_PRINT
	printf("Initialize()\r");
#endif

	SetOILED(PWM1_GREEN, 1);
	SetOILED(PWM2_GREEN, 1);
	SetOILED(RELAY1_GREEN, 1);
	SetOILED(RELAY2_GREEN, 1);
	SetOILED(SWITCH1_LED, 1);
	SetOILED(SWITCH2_LED, 1);
	SetOILED(SWITCH3_LED, 1);
	
	InitPressureSwitch(DIO_COMPRESSOR_SENSOR_PORT, RELAY_COMPRESSOR_PORT);
}

void Autonomous(void)
{
#ifdef DEBUG_PRINT
	printf("Autonomous()\r");
#endif
}
