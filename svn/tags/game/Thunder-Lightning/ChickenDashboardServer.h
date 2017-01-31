#pragma once
#include "Task.h"
#include <semLib.h>
#include "Synchronized.h"
#include "ErrorBase.h"
#include <vector>

#pragma pack (push, 1)
struct RectanglePacket
{
	UINT16 x;
	UINT16 y;
	UINT16 width;
	UINT16 height;
	UINT8 isTarget;
};
#pragma pack (pop)

enum CRIO_PACKET_DATA_TYPE
{
	CRIO_PACKET_DATA_TYPE_RECTANGLES,
	// UINT32            Number of rectangle packets
	// RectanglePacket[] Rectangles

	CRIO_PACKET_DATA_TYPE_OVERLAY_STRING,
	// UINT32 Overlay string length
	// INT8[] Overlay string

	CRIO_PACKET_DATA_TYPE_DASHBOARD_SELECT
	// UINT8 Dashboard selection (DASHBOARD_SELECTION)
	// if DASHBOARD_SELECTION_CAMERA:
	//	UINT8  Camera type (CAMERA_TYPE)
	//	UINT32 Camera IP string length
	//	INT8[] Camera IP string
	// if DASHBOARD_SELECTION_DIAGNOSTICS:
	//	UINT32 Diagnostics string length
	//	INT8[] Diagnostics string
};

enum CAMERA_TYPE
{
	CAMERA_TYPE_ARM,
	CAMERA_TYPE_MINIBOT
};

enum DASHBOARD_SELECTION
{
	DASHBOARD_SELECTION_CAMERA,
	DASHBOARD_SELECTION_DIAGNOSTICS
};

class ChickenDashboardServer
{
public:
	ChickenDashboardServer();
	~ChickenDashboardServer();

	inline void NotifyNewData()
	{ semGive(newDataSem); }
	
	inline void notifyNewDashboardSelection()
	{ updateDashboardSelection = true; }

	inline void notifyNewRectangles()
	{ updateRectangles = true; }

	inline void notifyNewOverlayString()
	{ updateOverlayString = true; }
	
	std::vector<RectanglePacket*>& GetRectangles()
	{ return rectangles; }
	
	inline SEM_ID getRectanglesSem()
	{ return rectanglesSem; }
	
	inline void setOverlayString(const char* string)
	{
		Synchronized sync(stringSem);
		strcpy(overlayString, string);
	}
	
	inline void setDashboardSelection(DASHBOARD_SELECTION selection)
	{ dashboardSelection = selection; }
	
	inline void setDashboardCameraIP(const char* cameraIP)
	{ dashboardCameraIP = cameraIP; }

	inline void setCameraType(CAMERA_TYPE cameraType)
	{ dashboardCameraTypeSelection = cameraType; }
	
	inline CAMERA_TYPE getCameraType()
	{ return dashboardCameraTypeSelection; }
	
	inline void setDiagnosticsString(const char* string)
	{
		Synchronized sync(stringSem);
		strcpy(diagnosticsString, string);
	}

private:
	static int s_ServerTask(ChickenDashboardServer *thisPtr);

	void ServerTask();
	void StartTask();
	void InitializeSockets();
	void CloseSockets();
	
	std::vector<RectanglePacket*> rectangles;
	char overlayString[0xFF];

	bool updateDashboardSelection;
	bool updateRectangles;
	bool updateOverlayString;

	DASHBOARD_SELECTION dashboardSelection;
	CAMERA_TYPE dashboardCameraTypeSelection;
	const char* dashboardCameraIP;
	char diagnosticsString[0x1000];

	Task serverTask;

	int listenSock;
	int pcSock;
	
	SEM_ID newDataSem;
	SEM_ID rectanglesSem;
	SEM_ID stringSem;
};
