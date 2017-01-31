#pragma once
#include "WPILib.h"
#include "UPanel.h"
#include "WPIObject.h"
#include "Config.h"
#include "ChickenDashboardServer.h"

// Forward declaration
class Control;
class CameraControl;
class LEDControl;
class ArmControl;

class ChickenRobot : public IterativeRobot
{
	CameraControl* cameraControl;
	LEDControl* ledControl;
	ArmControl* armControl;
	
	CAMERA_TYPE savedDashboardCamera;
	
    ChickenDashboardServer* dashboard;

    vector<WPIObjectBase*> wpiObjects;
    bool isDiagnosticsEnabled;
    Joystick* joysticks[3];
    vector<Control*> controls;
    UPanel* upanel;
    Compressor* compressor;

    unsigned int currentLine;
    int currentColumn;

    bool printing;
    bool hasPrinted;
    
    bool runAuto;

    Timer matchTimer;

public:
    ChickenRobot();
    
    virtual void RobotInit();
	virtual void DisabledInit();
	virtual void AutonomousInit();
	virtual void TeleopInit();

	virtual void DisabledPeriodic();
	virtual void AutonomousPeriodic();
	virtual void TeleopPeriodic();

	virtual void DisabledContinuous();
	virtual void AutonomousContinuous();
	virtual void TeleopContinuous();

	inline ChickenDashboardServer* getDashboard()
    { return dashboard; }

	inline void setJoysticks(bool isJoy1, bool isJoy2, bool isJoy3)
    {
        joysticks[0] = joysticks[1] = joysticks[2] = NULL;
        if (isJoy1)
            joysticks[0] = new Joystick(1, 4);
        if (isJoy2)
            joysticks[1] = new Joystick(2);
        if (isJoy3)
            joysticks[2] = new Joystick(3);
    }

	inline bool getJoystickTrigger(int port)
    { return getJoystickRawButton(port, 1); }

	inline bool getJoystickRawButton(int port, int button)
    { return joysticks[port - 1]->GetRawButton(button); }

	inline double getJoystickThrottle(int port)
    { return joysticks[port - 1]->GetThrottle(); }

	inline double getJoystickX(int port)
    { return joysticks[port - 1]->GetX(); }

	inline double getJoystickY(int port)
    { return joysticks[port - 1]->GetY(); }

	inline bool isJoystick(int port)
    { return joysticks[port - 1] != NULL; }

	inline void setPanelOffset(double offset)
    { upanel->setOffset(offset); }

	inline double getPanelAxis()
    { return upanel->getAxis(); }

	inline bool getPanelLeftButton()
    { return upanel->getLeftButton(); }

	inline bool getPanelMiddleButton()
    { return upanel->getMiddleButton(); }

	inline bool getPanelRightButton()
    { return upanel->getRightButton(); }

	inline bool getPanelTopSwitch()
    { return upanel->getTopSwitch(); }

	inline bool getPanelLeftSwitch()
    { return upanel->getLeftSwitch(); }

	inline bool getPanelRightSwitch()
    { return upanel->getRightSwitch(); }

	inline int getPanelKnob()
    { return upanel->getKnob(); }

	inline CANJaguar* addCANJaguar(int port, const char* name)
    {
    	CANJaguar* thing = new CANJaguar(port);
    	wpiObjects.push_back(new WPIObject<SpeedController, WPI_OBJECT_TYPE_SPEED_CONTROLLER>(this, name, thing));
    	return thing;
    }

	inline Victor* addVictor(int port, const char* name)
    {
    	Victor* thing = new Victor(port);
    	wpiObjects.push_back(new WPIObject<SpeedController, WPI_OBJECT_TYPE_SPEED_CONTROLLER>(this, name, thing));
    	return thing;
    }

	inline Jaguar* addJaguar(int port, const char* name)
    {
    	Jaguar* thing = new Jaguar(port);
    	wpiObjects.push_back(new WPIObject<SpeedController, WPI_OBJECT_TYPE_SPEED_CONTROLLER>(this, name, thing));
    	return thing;
    }

	inline Encoder* addEncoder(int port1, int port2, const char* name)
    {
    	Encoder* thing = new Encoder(port1,port2);
    	wpiObjects.push_back(new WPIObject<Encoder, WPI_OBJECT_TYPE_ENCODER>(this, name, thing));
    	return thing;
    }

	inline Solenoid* addSolenoid(int port, const char* name)
    {
    	Solenoid* thing = new Solenoid(port);
    	wpiObjects.push_back(new WPIObject<Solenoid, WPI_OBJECT_TYPE_SOLENOID>(this, name, thing));
    	return thing;
    }

	inline Servo* addServo(int port, const char* name)
    {
    	Servo* thing = new Servo(port);
    	wpiObjects.push_back(new WPIObject<Servo, WPI_OBJECT_TYPE_SERVO>(this, name, thing));
    	return thing;
    }

	inline DigitalInput* addDigitalInput(int port, const char* name)
    {
    	DigitalInput* thing = new DigitalInput(port);
    	wpiObjects.push_back(new WPIObject<DigitalInput, WPI_OBJECT_TYPE_DIGITAL_INPUT>(this, name, thing));
    	return thing;
    }
	
	inline AnalogChannel* addAnalogChannel(int port, const char* name)
    {
    	AnalogChannel* thing = new AnalogChannel(port);
    	wpiObjects.push_back(new WPIObject<AnalogChannel, WPI_OBJECT_TYPE_ANALOG_CHANNEL>(this, name, thing));
    	return thing;
    }

	inline double getMatchTimer()
    { return matchTimer.Get(); }
};
