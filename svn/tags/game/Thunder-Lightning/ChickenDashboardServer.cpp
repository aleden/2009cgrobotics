#include <vxWorks.h>
#include <errnoLib.h>
#include <hostLib.h>
#include <inetLib.h>
#include <sockLib.h>

#include "Vision/AxisCamera.h"
#include "Vision2009/BaeUtilities.h"
#include "Vision2009/FrcError.h"
#include "ChipObject/NiRioStatus.h"
#include "Task.h"
#include "Timer.h"
#include "Utility.h"
#include "Synchronized.h"

#include "ChickenDashboardServer.h"

#define DEBUG_PRINT
#ifndef DEBUG_PRINT
inline static void NULL_FUNC(...) {}
#define printf(...) NULL_FUNC(__VA_ARGS__)
#endif

/** port for sending video to laptop */
#define VIDEO_TO_PC_PORT 1180

ChickenDashboardServer::ChickenDashboardServer()
	: updateDashboardSelection(false), updateRectangles(false), updateOverlayString(false),
	serverTask("ChickenDashboardServer", (FUNCPTR)s_ServerTask),
	listenSock(0), pcSock(0), newDataSem(0)
{
	newDataSem = semBCreate(SEM_Q_PRIORITY, SEM_EMPTY);
	rectanglesSem = semMCreate(SEM_Q_PRIORITY | SEM_INVERSION_SAFE | SEM_DELETE_SAFE);
	stringSem = semMCreate(SEM_Q_PRIORITY | SEM_INVERSION_SAFE | SEM_DELETE_SAFE);
	StartTask();
}

ChickenDashboardServer::~ChickenDashboardServer()
{
	serverTask.Stop();
	CloseSockets();
}

void ChickenDashboardServer::CloseSockets()
{
	close(listenSock);
	close(pcSock);
	
	listenSock = pcSock = 0;
}

void ChickenDashboardServer::StartTask()
{ serverTask.Start((int)this); }

int ChickenDashboardServer::s_ServerTask(ChickenDashboardServer* thisPtr)
{ thisPtr->ServerTask(); return 0; }

static bool WaitAndWrite(int socket, const char* pointer, unsigned int size)
{
	unsigned int bytesSoFar = 0;
	unsigned int remaining = size;

	while (bytesSoFar < size)
	{
		int thisBytes = send(socket, pointer + bytesSoFar, remaining, 0);
		if (thisBytes == ERROR)
			return false;
		
		bytesSoFar += static_cast<unsigned int>(thisBytes);
		remaining -= static_cast<unsigned int>(thisBytes);
	}
	
	return true;
}

#define WRITE_PC_CHECK(POINTER, SIZE)\
	do\
	{\
		if (!WaitAndWrite(pcSock, (char*)(POINTER), (SIZE)))\
		{\
			CloseSockets();\
			goto Initialize;\
		}\
	} while (false)

#define WRITE_PC_STRING_CHECK(STRING)\
	do\
	{\
		UINT32 stringLength;\
		UINT32 netStringLength;\
		{\
			Synchronized sync(stringSem);\
			stringLength = strlen(STRING);\
		}\
		netStringLength = htonl(stringLength);\
		\
		WRITE_PC_CHECK(&netStringLength, 4);\
		\
		{\
			Synchronized sync(stringSem);\
			WRITE_PC_CHECK(STRING, stringLength);\
		}\
	} while (false)

void ChickenDashboardServer::ServerTask()
{
Initialize:
	printf("ChickenDashboardServer Initialize:\n");
	InitializeSockets();

	for (;;)
	{
		if (semTake(newDataSem, 1000) == ERROR)
			continue;
		
		printf(".");

		//
		// Send rectangles
		//
		if (updateRectangles)
		{
			UINT8 type = CRIO_PACKET_DATA_TYPE_RECTANGLES;

			WRITE_PC_CHECK(&type, 1);

			UINT32 numRectangles;
			{
				Synchronized sync(rectanglesSem);
				numRectangles = htonl(rectangles.size());
			}

			WRITE_PC_CHECK(&numRectangles, 4);

			{
				Synchronized sync(rectanglesSem);
				for (std::vector<RectanglePacket*>::iterator it = rectangles.begin(); it != rectangles.end(); ++it)
				{
					RectanglePacket rectangle;
					rectangle.x = htons((*it)->x);
					rectangle.y = htons((*it)->y);
					rectangle.width = htons((*it)->width);
					rectangle.height = htons((*it)->height);
		
					WRITE_PC_CHECK(&rectangle.x, 2);
					WRITE_PC_CHECK(&rectangle.y, 2);
					WRITE_PC_CHECK(&rectangle.width, 2);
					WRITE_PC_CHECK(&rectangle.height, 2);
					WRITE_PC_CHECK(&rectangle.isTarget, 1);
				}
			}
			
			updateRectangles = false;
		}
		
		//
		// Send overlay string
		//
		if (updateOverlayString)
		{
			UINT8 type = CRIO_PACKET_DATA_TYPE_OVERLAY_STRING;
			WRITE_PC_CHECK(&type, 1);
		
			WRITE_PC_STRING_CHECK(overlayString);
			
			updateOverlayString = false;
		}

		//
		// Send dashboard selection
		//
		if (updateDashboardSelection)
		{
			UINT8 type = CRIO_PACKET_DATA_TYPE_DASHBOARD_SELECT;
			WRITE_PC_CHECK(&type, 1);

			UINT8 dashboardSelectionByte = static_cast<UINT8>(dashboardSelection);
			WRITE_PC_CHECK(&dashboardSelectionByte, 1);
			
			switch (dashboardSelection)
			{
				case DASHBOARD_SELECTION_CAMERA:
				{
					UINT8 dashboardCameraTypeSelectionByte = static_cast<UINT8>(dashboardCameraTypeSelection);
					WRITE_PC_CHECK(&dashboardCameraTypeSelectionByte, 1);
					WRITE_PC_STRING_CHECK(dashboardCameraIP);
				}
				break;
				
				case DASHBOARD_SELECTION_DIAGNOSTICS:
				{
					WRITE_PC_STRING_CHECK(diagnosticsString);
				}
				break;
			}
			
			updateDashboardSelection = false;
		}
	}
}

void ChickenDashboardServer::InitializeSockets()
{
	struct sockaddr_in serverAddr;
	int sockAddrSize = sizeof(serverAddr);
	bzero ((char *) &serverAddr, sockAddrSize);
	serverAddr.sin_len = (u_char) sockAddrSize;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons (VIDEO_TO_PC_PORT);
	serverAddr.sin_addr.s_addr = htonl (INADDR_ANY);

	pcSock = listenSock = ERROR;

	for (;;)
	{
		// Create the socket.
		printf("ChickenDashboardServer: socket()\n");
		if ((listenSock = socket(AF_INET, SOCK_STREAM, 0)) == ERROR)
		{
			perror("ChickenDashboardServer: socket");
			continue;
		}
		
		printf("ChickenDashboardServer: setsockopt()\n");
		// Set the TCP socket so that it can be reused if it is in the wait state.
		int reuseAddr = 1;
		if (setsockopt(listenSock, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>(&reuseAddr), sizeof(reuseAddr)) == ERROR)
		{
			perror("setsockopt");
			close(listenSock);
			continue;
		}

		printf("ChickenDashboardServer: bind()\n");
		// Bind socket to local address.
		if (bind(listenSock, (struct sockaddr *) &serverAddr, sockAddrSize) == ERROR)
		{
			perror ("ChickenDashboardServer: bind");
			close(listenSock);
			continue;
		}

		printf("ChickenDashboardServer: listen()\n");
		// Create queue for client connection requests.
		if (listen(listenSock, 1) == ERROR)
		{
			perror ("ChickenDashboardServer: listen");
			close(listenSock);
			continue;
		}

		printf("ChickenDashboardServer: accept()\n");
		struct sockaddr_in clientAddr;
		int clientAddrSize;
		pcSock = accept (listenSock, reinterpret_cast<sockaddr*>(&clientAddr), &clientAddrSize);
		if (pcSock  == ERROR)
		{
			perror("ChickenDashboardServer: accept");
			close(listenSock);
			continue;
		}
		printf("ChickenDashboardServer: broke.\n");
		break;
	}
}
