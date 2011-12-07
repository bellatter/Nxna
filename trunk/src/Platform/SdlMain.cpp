#include "PlatformDefs.h"

#ifndef NXNA_PLATFORM_NACL

#include <SDL.h>

extern int NxnaMain(int argc, char* argv[]);

int main(int argc, char* argv[])
{
	return NxnaMain(argc, argv);
}

#endif
