#include "WPILib.h"
#include "ChickenRobot.h"
#include "WPIObject.h"
#include "Driver.h"
#include "MinibotControl.h"
#include "ArmControl.h"
#include "CameraControl.h"
#include "LEDControl.h"
#include <stdio.h>
#include <string.h>
#include "deadzone.h"
#include "Utils.h"

const char* wpiObjectTypeNames[] =
{
	"Speed controller",
	"Solenoid",
	"Servo",
	"Encoder",
	"Digital input",
	"Analog channel",
	"Compressor"
};

ChickenRobot::ChickenRobot() : savedDashboardCamera(CAMERA_TYPE_ARM), runAuto(false)
{
    isDiagnosticsEnabled = false;
    setJoysticks(true, true, false);

    dashboard = new ChickenDashboardServer();
	compressor = new Compressor(8, 1);
	upanel = new UPanel(3);

	controls.push_back(armControl = new ArmControl(this));
	controls.push_back(ledControl = new LEDControl(this));
	controls.push_back(cameraControl = new CameraControl(this, armControl, ledControl));
	controls.push_back(new Driver(this, cameraControl, armControl));
	controls.push_back(new MinibotControl(this));

	cameraControl->selectDashboardCamera(CAMERA_TYPE_ARM);}

void ChickenRobot::RobotInit()
{
}

void ChickenRobot::DisabledInit() {}

void ChickenRobot::AutonomousInit()
{
	compressor->Start();
	    
    for (std::vector<Control*>::iterator it = controls.begin() ; it != controls.end(); ++it)
        (*it)->autonomousInit();
}

void ChickenRobot::TeleopInit()
{
    compressor->Start();
    
    for (std::vector<Control*>::iterator it = controls.begin() ; it != controls.end(); ++it)
        (*it)->teleopInit();
}

void ChickenRobot::DisabledPeriodic()
{}

void ChickenRobot::AutonomousPeriodic()
{
	DriverStationLCD::GetInstance()->Clear();

	for (std::vector<Control*>::iterator it = controls.begin() ; it != controls.end(); ++it)
        (*it)->autonomousPeriodic();

	DriverStationLCD::GetInstance()->UpdateLCD();
}

void ChickenRobot::TeleopPeriodic()
{
	DriverStationLCD::GetInstance()->Clear();
    if (isDiagnosticsEnabled)
    {
    	char diagnosticsString[0x1000];
    	unsigned int diagnosticsStringLength = 0;

    	for (std::vector<WPIObjectBase*>::iterator it = wpiObjects.begin() ; it != wpiObjects.end(); ++it)
        {
            WPIObjectBase* o = *it;
            int joystickPort = o->getDiagnosticsTrigger() ? o->getDiagnosticsTrigger()->getJoystickPort() : -1;
	    	
            diagnosticsStringLength += sprintf(diagnosticsString + diagnosticsStringLength,
	    				"%s [%s] : ", o->getName(), wpiObjectTypeNames[o->getType()]);

    		switch (o->getType())
            {
                case WPI_OBJECT_TYPE_SPEED_CONTROLLER:
                {
                	SpeedController* speedController = ((WPIObject<SpeedController, WPI_OBJECT_TYPE_SPEED_CONTROLLER>*)o)->getWPIObject();
                	diagnosticsStringLength += sprintf(diagnosticsString + diagnosticsStringLength,
                			"%f", speedController->Get());
                	
                	if (o->getDiagnosticsTrigger()->isOn())
                		speedController->Set(deadzone(getJoystickY(joystickPort)));
                    break;
                }

                case WPI_OBJECT_TYPE_SOLENOID:
                {
                	Solenoid* solenoid = ((WPIObject<Solenoid, WPI_OBJECT_TYPE_SOLENOID>*)o)->getWPIObject();
                	diagnosticsStringLength += sprintf(diagnosticsString + diagnosticsStringLength,
                			"%d", solenoid->Get());

                	if (o->getDiagnosticsTrigger()->isOn())
                		solenoid->Set(getJoystickTrigger(joystickPort));
                    break;
                }

                case WPI_OBJECT_TYPE_ENCODER:
                {
                	Encoder* encoder = ((WPIObject<Encoder, WPI_OBJECT_TYPE_ENCODER>*)o)->getWPIObject();
                	diagnosticsStringLength += sprintf(diagnosticsString + diagnosticsStringLength,
                			"%d", encoder->Get());
                    break;
                }

                case WPI_OBJECT_TYPE_DIGITAL_INPUT:
                {
                	DigitalInput* digitalInput = ((WPIObject<DigitalInput, WPI_OBJECT_TYPE_DIGITAL_INPUT>*)o)->getWPIObject();
                	diagnosticsStringLength += sprintf(diagnosticsString + diagnosticsStringLength,
                			"%d", digitalInput->Get());
                    break;
                }
                
                case WPI_OBJECT_TYPE_ANALOG_CHANNEL:
                {
                	AnalogChannel* analogChannel = ((WPIObject<AnalogChannel, WPI_OBJECT_TYPE_ANALOG_CHANNEL>*)o)->getWPIObject();
                	diagnosticsStringLength += sprintf(diagnosticsString + diagnosticsStringLength,
                			"%d", analogChannel->GetValue());
                    break;
                }
                
                case WPI_OBJECT_TYPE_SERVO:
                {
                	Servo* servo = ((WPIObject<Servo, WPI_OBJECT_TYPE_SERVO>*)o)->getWPIObject();
                	diagnosticsStringLength += sprintf(diagnosticsString + diagnosticsStringLength,
                			"%f = %f (angle)", servo->Get(), servo->GetAngle());
                	
                	if (o->getDiagnosticsTrigger()->isOn())
                		servo->Set(getJoystickThrottle(joystickPort));
                	break;
                }

                case WPI_OBJECT_TYPE_COMPRESSOR:
                {
                	Compressor* compressor = ((WPIObject<Compressor, WPI_OBJECT_TYPE_COMPRESSOR>*)o)->getWPIObject();
                	diagnosticsStringLength += sprintf(diagnosticsString + diagnosticsStringLength,
                			"%d", compressor->GetPressureSwitchValue());
                	break;
                }
            }
    		
            if (o->getDiagnosticsTrigger())
            {
	    		diagnosticsStringLength += sprintf(diagnosticsString + diagnosticsStringLength,
	    				"(Joystick %d, Button %d)",
	    				o->getDiagnosticsTrigger()->getJoystickPort(),
	    				o->getDiagnosticsTrigger()->button);
	    		
	    		const char* methodString;
	    		
	    		switch (o->getType())
	            {
	                case WPI_OBJECT_TYPE_SPEED_CONTROLLER:
	                	methodString = " (Y Axis)";
	                    break;

	                case WPI_OBJECT_TYPE_SOLENOID:
	                	methodString = " (Trigger)";
	                    break;
	                
	                case WPI_OBJECT_TYPE_SERVO:
	                	methodString = " (Throttle)";
	                    break;
	                    
	                default:
	                	methodString = "";
	                    break;
	            }
	    		
	    		diagnosticsStringLength += sprintf(diagnosticsString + diagnosticsStringLength,
	    				methodString);
	    	}
    		
    		diagnosticsStringLength += sprintf(diagnosticsString + diagnosticsStringLength, "\n");
        }
    	
    	dashboard->setDiagnosticsString(diagnosticsString);
    	dashboard->setDashboardSelection(DASHBOARD_SELECTION_DIAGNOSTICS);
    	dashboard->notifyNewDashboardSelection();
    	dashboard->NotifyNewData();
    	
        isDiagnosticsEnabled = !getJoystickRawButton(1, 11);
        
        if (!isDiagnosticsEnabled)
        	cameraControl->selectDashboardCamera(savedDashboardCamera);
    }
    else
    {
		PRESSED_TRIGGER(testAutoTrigger, getJoystickRawButton(1, 3) &&
				getJoystickRawButton(1, 4));

		if (testAutoTrigger)
		{
			runAuto = !runAuto;
			if (runAuto)
    			AutonomousInit();
		}

		if (runAuto)
    	{
    		AutonomousPeriodic();
    	}
    	else
    	{
	        for (std::vector<Control*>::iterator it = controls.begin() ; it != controls.end(); ++it)
	            (*it)->teleopPeriodic();
	
	        isDiagnosticsEnabled = getJoystickRawButton(1, 12);
	        
	        if (isDiagnosticsEnabled)
	        	savedDashboardCamera = dashboard->getCameraType();
    	}
    }
	DriverStationLCD::GetInstance()->UpdateLCD();
}

void ChickenRobot::DisabledContinuous() {}
void ChickenRobot::AutonomousContinuous() {}
void ChickenRobot::TeleopContinuous() {}

START_ROBOT_CLASS(ChickenRobot);

