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

// create some macros to disable constant warnings about "override" keyword
#ifdef _MSC_VER
#define NXNA_DISABLE_OVERRIDE_WARNING __pragma(warning(push)) \
__pragma(warning(disable:4481))
#define NXNA_ENABLE_OVERRIDE_WARNING __pragma(warning(pop))
#else
#define NXNA_DISABLE_OVERRIDE_WARNING
#define NXNA_ENABLE_OVERRIDE_WARNING
#endif

// make sure the SDL libraries get included if needed
#if defined NXNA_PLATFORM_WIN32_SDL
#pragma comment(lib, "SDL")
#pragma comment(lib, "SDLMain")
#endif

#endif // NXNACONFIG_H
