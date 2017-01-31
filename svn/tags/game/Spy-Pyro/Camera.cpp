#include "WPILib.h"
#include "nivision.h"
#include "Vision/AxisCamera.h"
#include "Vision/HSLImage.h"
#include "Target.h"
#include "Camera.h"
#include "Math.h"

Camera* const camera = new Camera();

#define MINIMUM_SCORE 0.01

using namespace std;

void Camera::initCamera()
{
	//Wait(10);
	//setup();
}

void Camera::setup()
{
#if 0
	printf("setting up camera\n");
	hasInitedCam = true;
	AxisCamera& camera = AxisCamera::GetInstance();
	camera.WriteResolution(AxisCamera::kResolution_320x240);
	camera.WriteCompression(0);
	camera.WriteBrightness(0);
#endif
}

void Camera::updateTarget()
{
#if 0
	printf("good\n");
	AxisCamera& axisCamera = AxisCamera::GetInstance();

	// Find FRC targets in the image
	HSLImage* image = axisCamera.GetImage();
	vector<Target> targets = Target::FindCircularTargets(image);

	// Save memory
	delete image;

	if(targets.size() == 0 || targets[0].m_score < MINIMUM_SCORE)
	{
		if(targets.size() == 0)
			printf("No target found\n\n");
		else
			printf("No valid targets found, best score: %f\n", targets[0].m_score);
		targetAcquired = false;
	}
	else
	{
		target = targets[0];
		targetAcquired = true;
	}
#endif
}

float Camera::getDistance()
{
	float majorRadius = target.m_majorRadius, minorRadius = target.m_minorRadius;
	printf("Major radius: %f, Minor radius: %f\n", majorRadius, minorRadius);
	
	float slope = 0.43, intercept = 0.3; // experimentally determined constants
	return (1/minorRadius - intercept)/slope;
}