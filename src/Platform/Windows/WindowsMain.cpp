#include "../PlatformDefs.h"
#include <cstdlib>

#if defined NXNA_PLATFORM_WIN32 && !defined NXNA_PLATFORM_WIN32_SDL 

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

extern int NxnaMain(int argc, const char** argv);


INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    PSTR lpCmdLine, INT nCmdShow)
{
    return NxnaMain(__argc, (const char**)__argv);
}

#endif