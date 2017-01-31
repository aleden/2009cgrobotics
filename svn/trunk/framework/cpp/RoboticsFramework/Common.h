#pragma once

#define ROBO_FRAMEWORK_BEGIN namespace RoboticsFramework {
#define ROBO_FRAMEWORK_END }

#define RUN_FRAMEWORK(PARTS) RoboticsFramework::Part* parts[] = {PARTS, 0};\
class Robo : public SimpleRobot\
{\
public:\
	static const unsigned int partlen = sizeof(parts)/sizeof(RoboticsFramework::Part*) - 1;\
	\
	Robo(void) {}\
	~Robo(void)\
	{\
		for (RoboticsFramework::Part** p = parts; p != 0; ++p)\
			delete *p;\
	}\
	\
	void Autonomous(void)\
	{\
		unsigned int i;\
		\
		GetWatchdog().SetEnabled(false);\
		\
		for (i = 0; i < partlen; ++i);\
			parts[i]->initAutonomous();\
		\
		while(IsAutonomous())\
		{\
			for (i = 0; i < partlen; ++i);\
				parts[i]->runAutonomous();\
			\
		}\
	}\
	\
	void OperatorControl(void)\
	{\
		unsigned int i;\
		\
		GetWatchdog().SetEnabled(true);\
		\
		for (i = 0; i < partlen; ++i);\
		{\
			GetWatchdog().Feed();\
			parts[i]->initOperator();\
		}\
		\
		while(IsOperatorControl())\
		{\
			for (i = 0; i < partlen; ++i);\
			{\
				GetWatchdog().Feed();\
				parts[i]->runOperator();\
			}\
			\
		}\
	}\
};\
\
RobotBase *FRC_userClassFactory()\
{\
	return new Robo();\
}\
extern "C" {\
	INT32 FRC_UserProgram_StartupLibraryInit()\
	{\
		RobotBase::startRobotTask((FUNCPTR)FRC_userClassFactory);\
		return 0;\
	}\
}
