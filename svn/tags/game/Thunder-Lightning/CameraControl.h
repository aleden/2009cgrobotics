#pragma once
#include "WPILib.h"
#include "Control.h"

enum CAMERA_AUTONOMOUS_STATE
{
	CAS_OFF,
	CAS_VISION_PROCESSING
};

// Forward declaration
class LEDControl;
class ArmControl;

class CameraControl : public Control
{
public:
	enum TARGET_LOCATION
	{
		TL_LEFT,
		TL_RIGHT,
		TL_CENTER,
		TL_INVALID
	};
	enum TARGET_DISTANCE
	{
		TD_FAR,
		TD_NEAR,
		TD_SCORE,
		TD_INVALID
	};

    static const char* armCameraIP;
    static const char* minibotCameraIP;

private:
	const char* cameraTypeIPStrings[3];

    ChickenDashboardServer* dashboard;
    LEDControl* ledControl;
    
    Timer visionTimer;
    
    CAMERA_AUTONOMOUS_STATE autonomousState;
    
    bool canSeeTarget;
    bool isHighPeg;
    bool shouldImageProcess;

    TARGET_LOCATION targetLocation;
    TARGET_DISTANCE targetDistance;
    ArmControl* armControl;
public:
    CameraControl(ChickenRobot* chicken, ArmControl* armControl, LEDControl* ledControl);
    void teleopInit();
    void teleopPeriodic();
    void autonomousInit();
    void autonomousPeriodic();
    void DoImageProcessing(Image* image);
    
    inline void selectDashboardCamera(CAMERA_TYPE cameraType)
    {
		dashboard->setDashboardCameraIP(cameraTypeIPStrings[cameraType]);
    	dashboard->setDashboardSelection(DASHBOARD_SELECTION_CAMERA);
    	dashboard->setCameraType(cameraType);
    	dashboard->notifyNewDashboardSelection();
		dashboard->NotifyNewData();
    }
    
    inline bool isImageProcessing()
    { return shouldImageProcess; }
    
    inline void setImageProcess(bool shouldImageProcess)
    { this->shouldImageProcess = shouldImageProcess; }
    
    inline void setIsHighPeg(bool isHighPeg)
    { this->isHighPeg = isHighPeg; }
    
    inline TARGET_LOCATION getTargetLocation()
    { return targetLocation; }
    
    inline TARGET_DISTANCE getTargetDistance()
    { return targetDistance; }
    
    inline bool isTargetFound()
    { return canSeeTarget; }
    
	inline CAMERA_AUTONOMOUS_STATE getAutonomousState()
	{ return autonomousState; }
	
	inline void setAutonomousState(CAMERA_AUTONOMOUS_STATE state)
	{ autonomousState = state; }
	
	inline void nextAutonomousState()
	{ autonomousState = (CAMERA_AUTONOMOUS_STATE)(autonomousState + 1); }
};
