#ifndef NXNACONFIG_H
#define NXNACONFIG_H

#include "Platform/PlatformDefs.h"

typedef unsigned char byte;

// are we using OpenGL ES?
#if defined NXNA_PLATFORM_APPLE_IOS || defined NXNA_PLATFORM_NACL
#define USING_OPENGLES
#endif

// disable Visual C++ 2010 specific stuff if needed
#if _MSC_VER != 1600
#define nullptr 0
#define override
#endif

#endif // NXNACONFIG_H
