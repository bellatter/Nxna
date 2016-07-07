#include "../NxnaConfig.h"

#ifdef NXNA_PLATFORMENGINE_SDL

#if defined NXNA_PLATFORM_APPLE
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif

extern int NxnaMain(int argc, char* argv[]);

int main(int argc, char* argv[])
{
	return NxnaMain(argc, argv);
}

#endif
