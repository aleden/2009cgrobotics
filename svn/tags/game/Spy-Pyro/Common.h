#pragma once
#include "WPILib.h"

#define RUN_FRAMEWORK(...) \
class KILIMANJARO : public SimpleRobot\
{\
	private:\
	const unsigned int partlen;\
	Part* parts[0x20];\
public:\
	KILIMANJARO(void) : partlen(sizeof(parts)/sizeof(Part*) - 1) {\
		Part* partsTemp[0x20] = {__VA_ARGS__, 0};\
		memcpy(parts, partsTemp, sizeof(Part*)*0x20);\
	}\
	~KILIMANJARO(void)\
	{\
		for (Part** p = parts; p != 0; ++p)\
			delete *p;\
	}\
	\
	void Autonomous(void)\
	{\
		GetWatchdog().SetEnabled(false);\
		\
		for (Part** p = parts; p != 0; ++p)\
			(*p)->initAutonomous();\
		\
		while(IsAutonomous())\
		{\
			for (Part** p = parts; p != 0; ++p)\
				(*p)->runAutonomous();\
			\
		}\
	}\
	\
	void OperatorControl(void)\
	{\
		GetWatchdog().SetEnabled(false);\
		\
		for (Part** p = parts; p != 0; ++p)\
		{\
			/*GetWatchdog().Feed();*/\
			(*p)->initOperator();\
		}\
		\
		while(IsOperatorControl())\
		{\
			for (Part** p = parts; p != 0; ++p)\
			{\
				/*GetWatchdog().Feed();*/\
				(*p)->runOperator();\
			}\
			\
		}\
	}\
};\
\
RobotBase *FRC_userClassFactory()\
{\
	return new KILIMANJARO();\
}\
extern "C" {\
	INT32 FRC_UserProgram_StartupLibraryInit()\
	{\
		RobotBase::startRobotTask((FUNCPTR)FRC_userClassFactory);\
		return 0;\
	}\
}
