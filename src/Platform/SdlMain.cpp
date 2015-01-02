#include "PlatformDefs.h"

#if !defined NXNA_PLATFORM_NACL && (!defined NXNA_PLATFORM_WIN32 || defined NXNA_PLATFORM_WIN32_SDL) 

#if defined NXNA_PLATFORM_APPLE
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif

extern int NxnaMain(int argc, const char* argv[]);

int main(int argc, char* argv[])
{
	return NxnaMain(argc, argv);
}

#endif
