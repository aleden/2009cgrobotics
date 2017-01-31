// MAJOR NOTE: Camera is flipped 90 degrees on Epic and Fail

#include "camera.h"
#include <AxisCamera.h>
#include <PCVideoServer.h>
#include <VisionAPI.h>
#include <DriverStation.h>
#include "DriverStationLCD.h"
#include "vxWorks.h"
#include "wrappers.h"
#include "drive.h"
#include "AxisCamera.h" 
#include "BaeUtilities.h"
#include "FrcError.h"
#include "PCVideoServer.h"
#include "Target.h"
#include "util.h"
#include "Turret.h"

// Borrowed from TwoColorDemo.cpp
CameraRoutines::CameraRoutines() // TODO: test camera
{
	// Initialize particle analysis report
	memset(&par1, 0, sizeof(par1));
	memset(&par2, 0, sizeof(par2));
	memset(&par, 0, sizeof(par));
	
	/* image data for tracking - override default parameters if needed */
	/* recommend making PINK the first color because GREEN is more 
	 * subsceptible to hue variations due to lighting type so may
	 * result in false positives */
	// PINK
	sprintf (td1.name, "PINK");
	td1.hue.minValue = 220;   
	td1.hue.maxValue = 255;  
	td1.saturation.minValue = 75;   
	td1.saturation.maxValue = 255;      
	td1.luminance.minValue = 85;  
	td1.luminance.maxValue = 255;
	// GREEN
	sprintf (td2.name, "GREEN");
	td2.hue.minValue = 55;   
	td2.hue.maxValue = 125;  
	td2.saturation.minValue = 58;   
	td2.saturation.maxValue = 255;    
	td2.luminance.minValue = 92;  
	td2.luminance.maxValue = 255;
	
	// Set up debug output: 
	// DEBUG_OFF, DEBUG_MOSTLY_OFF, DEBUG_SCREEN_ONLY, DEBUG_FILE_ONLY, DEBUG_SCREEN_AND_FILE
	SetDebugFlag(DEBUG_SCREEN_ONLY);
	
	// Start the CameraTask
	// Possible values for resolution: k640x480, k320x240, k160x120
#ifndef DUMPER
	if(StartCameraTask(framesPerSecond, 0, k160x120, ROT_0) == -1)
	{
		printf("Failed to spawn camera task; exiting. Error code %s", 
				GetVisionErrorText(GetLastVisionError()));
	}
#endif
	//PCVideoServer *pc = new PCVideoServer();
//	pc->Start();
}

CameraRoutines::~CameraRoutines() {}
	
// Robot pursues its target; uses PID correction
// (takes offset from target and distance to target)
/*void CameraRoutines::adjustForImage(float xOffset, float distance)
{
	float xSpeed, ySpeed;
	
	xSpeed = pid.calc(xOffset, dt);
	
	if(xSpeed > 1)
		xSpeed = 1;
	else if(xSpeed < -1)
		xSpeed = -1;
	printf("xSpeed is: %f\n", xSpeed);
	
	if(distance > minDistance)
		ySpeed = yAdjustSpeed;
	else
		ySpeed = 0;
	
	driver->drive(xSpeed, ySpeed, false, false);
	Wait(dt);
}*/

// Adapted from TwoColorDemo
bool CameraRoutines::getTarget(float &x, float &y, float &d)
{
	float distance;
	static double savedImageTimestamp = 0.0;
	
	//static int foo = 0;
	
	DriverStation::Alliance a = DriverStation::GetInstance()->GetAlliance();
	SecondColorPosition pos;
	
	if (a == DriverStation::kRed) pos = LEFT;
	if (a == DriverStation::kBlue) pos = RIGHT;
	if (a == DriverStation::kInvalid) pos = RIGHT; // doesn't matter
	
	// BELOW = pink above green; ABOVE = green above pink;
	// LEFT = green to left; RIGHT = pink to left
	// For Epic/Fail, RIGHT = pink above green
	if(FindTwoColors(td1, td2, pos, &par, distance))
	{
		// This image has been processed already
		if(par.imageTimestamp == savedImageTimestamp)
		{
			printf("Stale image!\n");
			return false;
		}
		// New image to be processed
		else
		{
			// The target was recognized; save the timestamp
			savedImageTimestamp = par.imageTimestamp;
			//printf("Image timestamp: %lf\n", savedImageTimestamp);
			
			// after updated WPILib is distributed
			
			// Get target coordinates of the lower one
			/*if (pos == LEFT) {
				x = par1.center_mass_x_normalized;
				y = par1.center_mass_y_normalized;
			} else {
				x = par2.center_mass_x_normalized;
				y = par2.center_mass_y_normalized;
			}*/
			
			x = par.center_mass_x_normalized;
			y = par.center_mass_y_normalized;
			
			
			//printf("Target coordinates: (%f, %f)\n", x, y);
			
			//if (foo++ % 10 == 0)
			//	printf("(%f, %f, %f)\n", x, y,d);
			// Get distance
			d = convertToRealDistance(distance);
			
			//printf("width: %d\t", targetWidth);
			//printf("Distance is: %f\n", d);
			//printf("turret angle is: %f\n", turret->getAngle());
			// Print break for clarity
			//printf("----------\n");
			return true;
		}
	}
	// No image found
	else
	{
		//printf ("l\n");
		//DriverStationLCD::GetInstance()->Printf(DriverStationLCD::kMain_Line6,1,"lost it",x,y,d);
		//			DriverStationLCD::GetInstance()->UpdateLCD();
		return false;
	}
}