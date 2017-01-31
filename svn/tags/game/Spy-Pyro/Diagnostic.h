#ifndef DIAG_H
#define DIAG_H

#include "Part.h"

class Diagnostics : public Part
{
public:
	void runDiagnostics();
};

extern Diagnostics *const diagnostics;

#endif
