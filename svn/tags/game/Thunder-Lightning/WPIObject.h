#pragma once
#include "WPILib.h"
#include "DiagnosticsTrigger.h"

// Forward declaration
class ChickenRobot;

enum WPI_OBJECT_TYPE
{
	WPI_OBJECT_TYPE_SPEED_CONTROLLER,
	WPI_OBJECT_TYPE_SOLENOID,
	WPI_OBJECT_TYPE_SERVO,
	WPI_OBJECT_TYPE_ENCODER,
	WPI_OBJECT_TYPE_DIGITAL_INPUT,
	WPI_OBJECT_TYPE_ANALOG_CHANNEL,
	WPI_OBJECT_TYPE_COMPRESSOR
};

class WPIObjectBase
{
	WPI_OBJECT_TYPE type;
	const char* name;
	DiagnosticsTrigger* diagnosticsTrigger;

public:
    inline WPIObjectBase(WPI_OBJECT_TYPE type, const char* name, ChickenRobot* chicken) :
    	type(type), name(name)
    {
    	if (type <= WPI_OBJECT_TYPE_SERVO)
    		diagnosticsTrigger = new DiagnosticsTrigger(chicken);
    	else
    		diagnosticsTrigger = NULL;
    }
    
    inline virtual ~WPIObjectBase()
    { delete diagnosticsTrigger; }
    
    inline const char* getName()
    { return name; }
    
    inline DiagnosticsTrigger* getDiagnosticsTrigger()
    { return diagnosticsTrigger; }

    inline WPI_OBJECT_TYPE getType()
    { return type; }
};

template <typename T, WPI_OBJECT_TYPE TYPE>
class WPIObject : public WPIObjectBase
{
	T* wpiObject;

public:
    inline WPIObject(ChickenRobot* chicken, const char* name, T* wpiObject) :
    	WPIObjectBase(TYPE, name, chicken),  wpiObject(wpiObject) {}
    
    inline T* getWPIObject()
    { return wpiObject; }
};
