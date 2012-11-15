#include "PlatformDefs.h"

#if !defined NXNA_PLATFORM_NACL && (!defined NXNA_PLATFORM_WIN32 || defined NXNA_PLATFORM_WIN32_SDL) 

#include <SDL.h>

extern int NxnaMain(int argc, char* argv[]);

int main(int argc, char* argv[])
{
	return NxnaMain(argc, argv);
}

#endif
