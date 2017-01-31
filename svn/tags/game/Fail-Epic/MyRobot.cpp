#include "WPILib.h"
#include "PIDController.h"
#include "TrackAPI.h"
#include "AxisCamera.h"
#include "drive.h"
#include "camera.h"
#include "Accel2Vel.h"
#include "MyDashboard.h"
#include "wrappers.h"
#include "compile config.h"
#include "OI.h"
#include "Turret.h"
#include "Autonomous.h"
#include "AutoTarget.h"
#include "RobotActions.h"
#include "DriverStationLCD.h"

//////////////////////////////////////////////
// External pointers defined and Initialized below
// in RobotDemo::RobotDemo().
Wrapper* hal;
DriveRoutines* driver;
OIRoutines* oi;
CameraRoutines *cameraMan;
Turret *turret;
AutonomousRoutines *autonomous;
AutoTarget *autotargeting;
RobotActions *robotActions;

bool rampEle = true;

class RobotDemo : public SimpleRobot
{
	////////////////////////////////////
	// Threads
	Task rampThread;
#ifndef DUMPER
	Task autotargetThread;
#endif
    Task robotActionsThread;
    
public:
	RobotDemo(void) :
		rampThread("Ramp_Thread",ramp_motors),
#ifndef DUMPER
		autotargetThread("autotarget", run_autotarget),
#endif
		robotActionsThread("robot_actions_thread", run_robotActions)
        {
			GetWatchdog().SetExpiration(100);
			const int top = 6;
        	const int left = 8;
        	const int right = 7;
        	
        	DriverStation* sdriver = DriverStation::GetInstance(); 
        	bool t = sdriver->GetDigitalIn(top);
        	bool r = sdriver->GetDigitalIn(right);
        	bool l = sdriver->GetDigitalIn(left);
        	
        	autonomous 	 = new AutonomousRoutines(AutonomousParams(t,r,l));
			
			hal       	 = new Wrapper();
			robotActions = new RobotActions();
#ifndef DUMPER
			cameraMan 	 = new CameraRoutines();
			turret     	 = new Turret();
			autotargetThread.Start();
#endif
			driver    	 = new DriveRoutines();
			oi        	 = new OIRoutines();
			// YOU MUST START THE RAMP THREAD. OTHERWISE THE
        	// ELEVATOR AND INTAKE WILL NOT WORK.
        	rampThread.Start();
        	robotActionsThread.Start();
        }

        void Autonomous(void)
        {
        	cout << "entering autonomous" << endl;
        	robotActions->stopFiring(); // why the hell does it fire in autonomous?
        	// Enable all autofiring features, and maximize panning range for maximum win
#ifndef DUMPER
#ifdef FAIL
        	autotargeting->panLimit = 20; // pathetic
#endif
#ifdef EPIC
        	autotargeting->panLimit = 60;
#endif

        	autotargeting->enableAutoRotate();
        	autotargeting->enableAutoPan();
        	autotargeting->enableAutoFire();
#endif
        	robotActions->stopFiring(); // why the hell does it fire in autonomous?
        	robotActions->elevatorStop();
        	robotActions->stopBlowing();
        	robotActions->off();
        	robotActions->enable();
        	cout << "enabled full auto firing" << endl;
        	GetWatchdog().SetEnabled(false);
        	while(IsAutonomous()) {
        		autonomous->run_autonomous();
        		Wait(0.1);
        	}
        }

        void OperatorControl(void)
        { int foo;
        	cout << "entering OC" << endl;
#ifndef DUMPER
#ifdef FAIL
        	autotargeting->panLimit = 30;
#endif
#ifdef EPIC
        	//autotargeting->panLimit = 30;
        	autotargeting->panLimit = 10;
#endif
        	autotargeting->disableAutoRotate();
#endif
        	GetWatchdog().SetEnabled(false);
        	while (IsOperatorControl()) {
       		Wait(0.01);
       		
       		if (hal->getJoystickButton(J_1, 8) && hal->getJoystickButton(J_1, 9))
       		{
       			robotActions->disable();
       			oi->testMotors();
       			DriverStationLCD::GetInstance()->Printf(DriverStationLCD::kMain_Line6,
       			1, "---DIAGNOSTIC MODE---");
       			DriverStationLCD::GetInstance()->UpdateLCD();
       			
       			rampEle = false;
       		}
       		else
       		{
       			robotActions->enable();
       			oi->operatorControlRoutine();
       			//DriverStationLCD::GetInstance()->Printf(DriverStationLCD::kMain_Line6,
       			//1, "-------OI MODE-------");
       			//DriverStationLCD::GetInstance()->UpdateLCD();
       			
       			rampEle = true;
#ifndef DUMPER
       			autotargeting->panCenter = turret->getAngle(); // the setpoint = the knob
#endif
       		}
       		driver->drive(hal->getJoystickX(J_1),hal->getJoystickY(J_1),true,true);
       		
       		foo ++;
       		/*if (foo > 50)
       		{
       			printf ("L = %f,%f,%f\t\tR = %f,%f,%f\n",
       			  hal->getWheelVelocity(SIDE_LEFT,OMNI),
       			  hal->getWheelVelocity(SIDE_LEFT,TRANSMISSION),
       			  hal->getMotorLeft(),
       			  hal->getWheelVelocity(SIDE_RIGHT,OMNI),
       			  hal->getWheelVelocity(SIDE_RIGHT,TRANSMISSION),
       			  hal->getMotorRight());
       			printf ("Throttle = %f\n",hal->getJoystickThrottle(J_1));
       			//printf("turretAngle = %f\n",hal->getTurretAngle());
       			//printf("turretSpeed = %f\n",hal->getMotorSpeed(M_TURRET));
       			foo = 0;
       		}*/
        	}
        	
        	/*int foo = 0;
        	while (IsOperatorControl())
        	{
        		GetWatchdog().Feed();

        		//cout << "right = " << hal->getMotorSpeed(M_RIGHT) << endl;
        		
        		//currentVel = hal->getWheelVelocity(SIDE_LEFT,OMNI);
        		foo ++;
        		if (foo > 5000)
        		{
        			/*printf ("L = %f,%f,%f\t\tR = %f,%f,%f\n",
        			  hal->getWheelVelocity(SIDE_LEFT,OMNI),
        			  hal->getWheelVelocity(SIDE_LEFT,TRANSMISSION),
        			  hal->getMotorLeft(),
        			  hal->getWheelVelocity(SIDE_RIGHT,OMNI),
        			  hal->getWheelVelocity(SIDE_RIGHT,TRANSMISSION),
        			  hal->getMotorRight());*/
        			/*foo = 0;
        		}
        		
        	}*/
        	//StopImageAcquisition();
        	//StopCameraTask();
        }
};

START_ROBOT_CLASS(RobotDemo);

