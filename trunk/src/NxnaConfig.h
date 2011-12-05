#ifndef NXNACONFIG_H
#define NXNACONFIG_H

#include "Platform/PlatformDefs.h"

typedef unsigned char byte;

// are we using OpenGL ES?
#ifdef NXNA_PLATFORM_APPLE_IPHONE
#define USING_OPENGLES
#elif defined NXNA_PLATFORM_IPHONE_SIMULATOR
#define USING_OPENGLES
#elif defined NXNA_PLATFORM_NACL
#define USING_OPENGLES
#endif

// disable Visual C++ 2010 specific stuff if needed
#if _MSC_VER != 1600
#define nullptr 0
#define override
#endif

#endif // NXNACONFIG_H