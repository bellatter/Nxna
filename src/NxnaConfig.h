#ifndef NXNACONFIG_H
#define NXNACONFIG_H

#include "Platform/PlatformDefs.h"

typedef unsigned char byte;

// are we using OpenGL ES?
#if defined NXNA_PLATFORM_APPLE_IOS || defined NXNA_PLATFORM_NACL || defined NXNA_PLATFORM_ANDROID
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

// if for some reason you don't want Ogg Vorbis (.ogg files) support you can uncomment the following line
//#define NXNA_DISABLE_OGG

// if you don't want Direct3D 11 support you can uncomment the following line
//#define NXNA_DISABLE_D3D11

#if defined NXNA_PLATFORM_APPLE_IOS
#define NXNA_DISABLE_OGG
#endif

#if !defined NXNA_PLATFORM_WIN32
#define NXNA_DISABLE_D3D11
#endif

#endif // NXNACONFIG_H
