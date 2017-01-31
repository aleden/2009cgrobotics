#include "RoboticsFramework.h"
#include "camera.h"
#include "control.h"
#include "script.h"
#include "locator.h"
#include "driver.h"
#include "hal.h"
#include "hanger.h"
#include "diagnostic.h"
#include "suckControl.h"

//RUN_FRAMEWORK(camera, control, script, locator, driver, hanger, hal)

bool isDiagnosticsMode;

class KILIMANJARO : public SimpleRobot
{
public:
	Part* parts[20];
	
	KILIMANJARO(void)
	{
		GetWatchdog().SetEnabled(false);

		isDiagnosticsMode = false;
#if 1
		Part* partsTemp[20] =
		{
			control, script, locator, driver,
			hanger, sucker, camera, diagnostics, hal, 
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		};
		memcpy(parts, partsTemp, sizeof(Part*)*20);
#else
		parts[0] = control;
		parts[1] = script;
		parts[2] = locator;
		parts[3] = driver;
		parts[4] = hanger;
		parts[5] = sucker;
		parts[6] = camera;
		parts[7] = diagnostics;
		parts[8] = hal;
#endif
		parts[9] = 0;
		parts[10] = 0;
	}
	~KILIMANJARO(void)
	{
		for (Part** p = parts; *p != 0; ++p)
			delete *p;
	}
	
	void Disabled(void)
	{
		GetWatchdog().SetEnabled(false);
		
		printf("Disabled()\n");
		
		while (IsDisabled())
		{
			for (Part** p = parts; *p != 0; ++p)
				(*p)->runDisabled();
			Wait(0.01);
		}
	}
	
	void Autonomous(void)
	{
		GetWatchdog().SetEnabled(false);

		for (Part** p = parts; *p != 0; ++p)
			(*p)->initAutonomous();
		
		while (IsAutonomous())
		{
			for (Part** p = parts; *p != 0; ++p) {
				(*p)->runAutonomous();
				GetWatchdog().Feed();
			}
			Wait(0.05);
			
		}
	}
	
	void OperatorControl(void)
	{
		GetWatchdog().SetEnabled(false);

		//GetWatchdog().SetExpiration(10.0);
		//GetWatchdog().SetEnabled(true);

		for (Part** p = parts; *p != 0; ++p)
		{
			GetWatchdog().Feed();
			printf("About to init %s\n", (*p)->getName() );
			(*p)->initOperator();
			printf("Just initd %s\n", (*p)->getName());
		}
		
		while (IsOperatorControl())
		{
			for (Part** p = parts; *p != 0; ++p)
			{
				GetWatchdog().Feed();
				//printf("%c", (*p)->getName()[0] );
				if (isDiagnosticsModeEnabled())
					(*p)->runDiagnostics();
				else
					(*p)->runOperator();
				//printf("%c", (*p)->getName()[0]);
			}
			Wait(0.05);
		}
	}
};

RobotBase* FRC_userClassFactory()
{
	return new KILIMANJARO();
}
extern "C"
{
	INT32 FRC_UserProgram_StartupLibraryInit()
	{
		RobotBase::startRobotTask((FUNCPTR)FRC_userClassFactory);
		return 0;
	}
}
