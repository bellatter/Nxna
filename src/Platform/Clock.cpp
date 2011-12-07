#include "Clock.h"
#include "../NxnaConfig.h"
#ifdef NXNA_PLATFORM_NACL
#include <sys/time.h>
#else
#include <SDL.h>
#endif

namespace Nxna
{
namespace Platform
{
	unsigned int Clock::GetCurrentTime()
	{
#ifdef NXNA_PLATFORM_NACL
		static bool haveStart = false;
		static timeval start;

		if (haveStart == false)
		{
			gettimeofday(&start, nullptr);
			haveStart = true;
		}

		unsigned int ticks;
		struct timeval now;

		gettimeofday(&now, nullptr);
		ticks =
			(now.tv_sec - start.tv_sec) * 1000 + (now.tv_usec -
												  start.tv_usec) / 1000;
		return (ticks);
#else
		return SDL_GetTicks();
#endif
	}
}
}
