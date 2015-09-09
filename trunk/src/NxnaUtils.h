#ifndef NXNAUTILS_H
#define NXNAUTILS_H

// This file is for "extra" parts of NXNA that aren't part of XNA, but are still handy.

#include "Content/FileStream.h"
#include "Utils/StopWatch.h"

namespace Nxna
{
namespace Utils
{
	unsigned int GetUTF8Character(const char* string, int position, unsigned int* result);
}
}

#endif // NXNAUTILS_H
