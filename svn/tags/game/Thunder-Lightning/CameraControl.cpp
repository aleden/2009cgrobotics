#include "CameraControl.h"
#include "LEDControl.h"
#include "ArmControl.h"
#include <math.h>
#include <algorithm>
#include "Utils.h"
#include "UserInterface.h"
#include "PIDController.h"

using namespace std;

const char* CameraControl::armCameraIP;
const char* CameraControl::minibotCameraIP;

CameraControl::CameraControl(ChickenRobot* chicken, ArmControl* armControl, LEDControl* ledControl) :
	Control(chicken),
    ledControl(ledControl), canSeeTarget(false),
	isHighPeg(true), shouldImageProcess(false),
	targetLocation(TL_INVALID), targetDistance(TD_INVALID),
	armControl(armControl)
{
#if CHICKEN_ROBOT == CHICKEN_LIGHTNING
    armCameraIP = "10.15.40.40";
    minibotCameraIP = "10.15.40.41";
#elif CHICKEN_ROBOT == CHICKEN_THUNDER
    armCameraIP = "10.15.41.40";
    minibotCameraIP = "10.15.41.41";
#endif

    cameraTypeIPStrings[CAMERA_TYPE_ARM] = armCameraIP;
    cameraTypeIPStrings[CAMERA_TYPE_MINIBOT] = minibotCameraIP;

	dashboard = getDashboard();

    visionTimer.Start();
    visionTimer.Reset();
}

void CameraControl::teleopInit()
{
    ledControl->setCameraLEDs(true);
}

void CameraControl::teleopPeriodic()
{
	RELEASED_TRIGGER(cameraView1Trigger, getPanelLeftButton());

	if (cameraView1Trigger)
		selectDashboardCamera(CAMERA_TYPE_ARM);
	
	RELEASED_TRIGGER(cameraView2Trigger, getPanelMiddleButton());

	if (cameraView2Trigger)
		selectDashboardCamera(CAMERA_TYPE_MINIBOT);
	
	if (getJoystickRawButton(UserInterface::PILOT_JOYSTICK_PORT, 12))
	{
		autonomousPeriodic();
		setImageProcess(true);
	}
	else
	{
		setImageProcess(false);
	}
}

void CameraControl::autonomousInit()
{
    ledControl->setCameraLEDs(true);
	setAutonomousState(CAS_OFF);
	selectDashboardCamera(CAMERA_TYPE_ARM);
}

void CameraControl::autonomousPeriodic()
{
#if 0
	static unsigned int counter = 0;
	
	if (shouldImageProcess && counter > 2)
	{
		if (visionTimer.Get() > /*.15*/.20)
		{
		    ledControl->setCameraLEDs(true);
		    Image* imaqImage = pegCamera->GetImage();
		    visionTimer.Start();
		    visionTimer.Reset();
			DoImageProcessing(imaqImage);
			DriverStationLCD::GetInstance()->Clear();
			DriverStationLCD::GetInstance()->Printf(DriverStationLCD::kUser_Line2, 1, "time:%f", (float)visionTimer.Get());
			DriverStationLCD::GetInstance()->UpdateLCD();
			imaqDispose(imaqImage);
			counter = 0;
			visionTimer.Reset();
		}
	}
	++counter;
#endif
}

static inline int ParticleMeasurement(double& outMeasurement, Image* image, int particleNumber, MeasurementType whatToMeasure)
{ return imaqMeasureParticle(image, particleNumber, 0, whatToMeasure, &outMeasurement); }

static inline int CalculateRectArea(const Rect& a)
{ return a.width * a.height; }

static inline bool CompareParticleSizes(const Rect& a, const Rect& b)
{ return CalculateRectArea(a) > CalculateRectArea(b); }

void CameraControl::DoImageProcessing(Image* image)
{
	static Image* lastBinaryImage = NULL;
	static bool thisBinaryImageLighted = false;

	//
	// Crop image to remove top and bottom (focus on peg)
	//
	ImageType imageType;
	imaqGetImageType(image, &imageType);

	int imageWidth, imageHeight;
	imaqGetImageSize(image, &imageWidth, &imageHeight);

	Image* croppedImage = imaqCreateImage(imageType, DEFAULT_BORDER_SIZE);
	imaqSetImageSize(croppedImage, imageWidth, static_cast<int>(static_cast<unsigned int>(imageHeight) / 2));

	Rect croppedRect;
	croppedRect.left = 0;
	croppedRect.width = imageWidth;
	croppedRect.top = static_cast<int>(static_cast<unsigned int>(imageHeight) / 4);
	croppedRect.height = static_cast<int>(static_cast<unsigned int>(imageHeight) / 2);

	Point point;
	point.x = point.y = 0;

	imaqCopyRect(croppedImage, image, croppedRect, point);

	int croppedImageWidth, croppedImageHeight;
	imaqGetImageSize(image, &croppedImageWidth, &croppedImageHeight);

	//
	// Apply HSL threshold to find bright targets
	//
	Image* thisBinaryImage = imaqCreateImage(IMAQ_IMAGE_U8, DEFAULT_BORDER_SIZE);

	Range hueRange = {0, 48};
	Range saturationRange = {0, 255};
	Range luminanceRange = {85, 255};

	imaqColorThreshold(thisBinaryImage, croppedImage, 1, IMAQ_HSL, &hueRange, &saturationRange, &luminanceRange);

	int thisBinaryImageWidth, thisBinaryImageHeight;
	imaqGetImageSize(image, &thisBinaryImageWidth, &thisBinaryImageHeight);

	if (!lastBinaryImage || !thisBinaryImageLighted)
	{
#if 0
		thisParticles = thisBinaryImage->GetOrderedParticleAnalysisReports();
		thisParticles->resize(6);
#endif
	}
	else
	{
		// Filter algorithm:
		// a (Non Lighted) | b (Lighted) | c (Output) | Reason
		// ---------------------------------------------------------------------
		//        1        |      1      |     0      | Was not reflective tape
		//        0        |      1      |     1      | Was reflective tape
		//        0        |      0      |     0      | Nothing
		//        1        |      0      |     0      | Image shifted
		//
		// Solution:
		// c = AND(NOT(a), b)
		
		imaqNand(lastBinaryImage, lastBinaryImage, lastBinaryImage);
		imaqAnd(thisBinaryImage, thisBinaryImage, lastBinaryImage);

		//
		// Aquire particles, sort by size
		//
		int numParticles;
		imaqCountParticles(thisBinaryImage, TRUE, &numParticles);
	
		vector<Rect> particleRects(numParticles);
	
		for (int particleNumber = 0; particleNumber < numParticles; ++particleNumber)
		{
			Rect& rect = particleRects[particleNumber];
	
			double x, y, w, h;
			ParticleMeasurement(x, thisBinaryImage, particleNumber, IMAQ_MT_BOUNDING_RECT_LEFT);
			ParticleMeasurement(y, thisBinaryImage, particleNumber, IMAQ_MT_BOUNDING_RECT_TOP);
			ParticleMeasurement(w, thisBinaryImage, particleNumber, IMAQ_MT_BOUNDING_RECT_WIDTH);
			ParticleMeasurement(h, thisBinaryImage, particleNumber, IMAQ_MT_BOUNDING_RECT_HEIGHT);
			
			rect.left = static_cast<int>(x);
			rect.top = static_cast<int>(y);
			rect.width = static_cast<int>(w);
			rect.height = static_cast<int>(h);
		}
	
		char overlayString[0xFF];
		unsigned int overlayStringLength = sprintf(overlayString, "Particles: %d\n", (int)particleRects.size());
	
		sort(particleRects.begin(), particleRects.end(), CompareParticleSizes);
		particleRects.resize(10);
	
		//
		// Set dashboard data
		//
		{
			Synchronized sync(dashboard->getRectanglesSem());
	
			//
			// Clear dashboard rectangles
			//
			for (std::vector<RectanglePacket*>::iterator it = dashboard->GetRectangles().begin(); it != dashboard->GetRectangles().end(); ++it)
				delete *it;
			
			dashboard->GetRectangles().clear();
			
			//
			// Add dashboard rectangles
			//
			for (std::vector<Rect>::iterator it = particleRects.begin(); it != particleRects.end(); ++it)
			{
				Rect& particleRect = *it;
	
				RectanglePacket* rectangle = new RectanglePacket();
				rectangle->isTarget = false;
				rectangle->x = particleRect.left;
				rectangle->y = particleRect.top;
				rectangle->width = particleRect.width;
				rectangle->height = particleRect.height;
				dashboard->GetRectangles().push_back(rectangle);
			}
			dashboard->notifyNewRectangles();
		}
		dashboard->NotifyNewData();
		
		//
		// Process particles, find targets
		//
		Rect* firstRect = NULL;
		Rect* secondRect = NULL;
		Rect* thirdRect = NULL;
		for (std::vector<Rect>::iterator it = particleRects.begin(); it != particleRects.end(); ++it)
		{
			Rect& thisRect = *it;
			
			if (thisRect.height > thisRect.width)
			{
				if (firstRect == NULL)
				{
					firstRect = &thisRect;
				}
				else if (secondRect == NULL)
				{
					if (firstRect->left - 100 < thisRect.left && firstRect->left + 100 > thisRect.left)
					{
						secondRect = &thisRect;
					}
				}
				else
				{
					if ((firstRect->left - 100 < thisRect.left && firstRect->left + 100 > thisRect.left) && (secondRect->left - 100 < thisRect.left && secondRect->left + 100 > thisRect.left))
					{
						thirdRect = &thisRect;
					}
				}
			}
		}
		
		// Sort rects by y position, find middle
		int firstY = min(min(firstRect->top, secondRect->top), thirdRect->top);
		if (firstRect->top == firstY)
		{
		}
		else if (secondRect->top == firstY)
		{
			Rect_struct* newFirst = secondRect;
			secondRect = firstRect;
			firstRect = newFirst;
		}
		else
		{
			Rect_struct* newFirst = thirdRect;
			thirdRect = firstRect;
			firstRect = newFirst;
		}
	
		int thirdY = max(max(firstRect->top, secondRect->top), thirdRect->top);
		if (firstRect->top == thirdY)
		{
			Rect_struct* newThird = firstRect;
			firstRect = thirdRect;
			thirdRect = newThird;
		}
		else if (secondRect->top == thirdY)
		{
			Rect_struct* newThird = secondRect;
			secondRect = thirdRect;
			thirdRect = newThird;
		}
		else
		{
		}
	
		//
		// Have we identified any of the target's rectangles?
		//
		if (firstRect != NULL || secondRect != NULL || thirdRect != NULL)
		{
			//
			// Set dashboard data
			//
			{
				Synchronized sync(dashboard->getRectanglesSem());
		
				//
				// Clear dashboard rectangles
				//
				for (std::vector<RectanglePacket*>::iterator it = dashboard->GetRectangles().begin(); it != dashboard->GetRectangles().end(); ++it)
					delete *it;
				
				dashboard->GetRectangles().clear();
				
				//
				// Add dashboard rectangles
				//
				if (firstRect != NULL)
				{
					RectanglePacket* rectangle = new RectanglePacket();
					rectangle->isTarget = true;
					rectangle->x = firstRect->left;
					rectangle->y = firstRect->top;
					rectangle->width = firstRect->width;
					rectangle->height = firstRect->height;
					dashboard->GetRectangles().push_back(rectangle);
				}
				if (secondRect != NULL)
				{
					RectanglePacket* rectangle = new RectanglePacket();
					rectangle->isTarget = true;
					rectangle->x = secondRect->left;
					rectangle->y = secondRect->top;
					rectangle->width = secondRect->width;
					rectangle->height = secondRect->height;
					dashboard->GetRectangles().push_back(rectangle);
				}
				if (thirdRect != NULL)
				{
					RectanglePacket* rectangle = new RectanglePacket();
					rectangle->isTarget = true;
					rectangle->x = thirdRect->left;
					rectangle->y = thirdRect->top;
					rectangle->width = thirdRect->width;
					rectangle->height = thirdRect->height;
					dashboard->GetRectangles().push_back(rectangle);
				}

				dashboard->notifyNewRectangles();
			}
			dashboard->NotifyNewData();

			canSeeTarget = true;
			
			int middleWidth = max(max(firstRect != NULL ? firstRect->width : -1, secondRect != NULL ? secondRect->width : -1), thirdRect != NULL ? thirdRect->width : -1);
			int middleX;
			if (middleWidth == firstRect->width)
				middleX = firstRect->left;
			else if (middleWidth == secondRect->width)
				middleX = secondRect->left;
			else if (middleWidth == thirdRect->width)
				middleX = thirdRect->left;
			
			int offset;
			
	#if CHICKEN_ROBOT == CHICKEN_THUNDER
			offset = static_cast<int>(0.21666666666667f * static_cast<float>(imageWidth));
	#elif CHICKEN_ROBOT == CHICKEN_LIGHTNING
			offset = static_cast<int>(0.1791666666666667f * static_cast<float>(imageWidth));
	#endif
			
			if (middleX + middleWidth/2 + offset < imageWidth/2)
				targetLocation = TL_LEFT;
			else if (middleX  + middleWidth/2 + offset > imageWidth/2)
				targetLocation = TL_RIGHT;
			else
				targetLocation = TL_CENTER;
			
			int farWidth, scoreWidth;
			
			if (isHighPeg)
			{
	#if 0
				farWidth = 13;
				scoreWidth = 15;
	#else
				farWidth = static_cast<int>((29.0 / 640.0) * static_cast<double>(imageWidth));
				scoreWidth = static_cast<int>((33.0 / 640.0) * static_cast<double>(imageWidth));
	#endif
			}
			else
			{
	#if 0
				farWidth = 12;
				scoreWidth = 10;
	#else
				farWidth = static_cast<int>(0.0541666666666667 * static_cast<double>(imageWidth));
				scoreWidth = static_cast<int>(0.0625 * static_cast<double>(imageWidth));
	#endif
			}
	
			if (middleWidth < farWidth)
				targetDistance = TD_FAR;
			else if (middleWidth <= scoreWidth)
				targetDistance = TD_SCORE;
			else
				targetDistance = TD_NEAR;
	
			static const char* targetLocationStrings[] =
			{
				"TL_LEFT",
				"TL_RIGHT",
				"TL_CENTER",
				"TL_INVALID"
			};
			static const char* targetDistanceStrings[] =
			{
				"TD_FAR",
				"TD_NEAR",
				"TD_SCORE",
				"TD_INVALID"
			};
	
			overlayStringLength += sprintf(overlayString + overlayStringLength,
					"Target:\n  Middle Width: %d\n  Location: %s\n  Distance: %s\nArm: %f -> %f",
					middleWidth,
					targetLocationStrings[getTargetLocation()],
					targetDistanceStrings[getTargetDistance()],
					(float)armControl->armJaguarEncoder->PIDGet(),
					(float)armControl->pidController->GetSetpoint());
			dashboard->setOverlayString(overlayString);
			dashboard->notifyNewOverlayString();
		}
		else
		{
			//
			// Report missing target rectangles
			//
			canSeeTarget = false;
			targetLocation = TL_INVALID;
			targetDistance = TD_INVALID;
			printf("Could not find vision target\n");
		}
	}
	
	if (croppedImage)
		imaqDispose(croppedImage);
	if (lastBinaryImage)
		imaqDispose(lastBinaryImage);
	
	thisBinaryImageLighted = !thisBinaryImageLighted;
	ledControl->setCameraLEDs(thisBinaryImageLighted);
	
	lastBinaryImage = thisBinaryImage;
}
