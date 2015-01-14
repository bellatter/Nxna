#ifndef NXNA_UTILS_STOPWATCH_H
#define NXNA_UTILS_STOPWATCH_H

#include <cstdint>

namespace Nxna
{
namespace Utils
{
	class StopWatch
	{
		bool m_running;
		uint64_t m_timeElapsed;
		uint64_t m_timeStarted;
		static uint64_t m_frequency;

	public:
		static uint64_t GetCurrentTicks();

		StopWatch();
		void Start();
		void Stop();
		void Reset();

		uint64_t GetElapsedTicks();
		uint64_t GetElapsedMilliseconds();
		unsigned int GetElapsedMilliseconds32();

	private:
		void getFrequency();
	};
}
}

#endif // NXNA_UTILS_STOPWATCH_H
