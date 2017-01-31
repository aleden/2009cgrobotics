#pragma once
#include "Common.h"
#include "CompileConfig.h"
#include "Part.h"

extern bool isDiagnosticsMode;

inline bool isDiagnosticsModeEnabled()
{ return isDiagnosticsMode; }

inline void setDiagnosticsModeEnabled(bool b)
{
	isDiagnosticsMode = b;
	//printf("isDiagnosticsMode = %d\n", (int)b);
}
