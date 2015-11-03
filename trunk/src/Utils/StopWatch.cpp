#include "StopWatch.h"


#ifdef _WIN32
#include <Windows.h>
#elif defined NXNA_PLATFORM_APPLE
#include <mach/mach.h>
#include <mach/mach_time.h>
#endif

namespace Nxna
{
namespace Utils
{
#ifdef _WIN32
	uint64_t StopWatch::m_frequency = 0;
#elif defined NXNA_PLATFORM_APPLE
	static mach_timebase_info_data_t m_info;
#endif
	
	uint64_t StopWatch::GetCurrentTicks()
	{
#ifdef _WIN32
		LARGE_INTEGER ticks;
		QueryPerformanceCounter(&ticks);
		return (uint64_t)ticks.QuadPart;
#elif defined NXNA_PLATFORM_APPLE
		return mach_absolute_time();
#endif
	}

	StopWatch::StopWatch()
	{
		m_running = false;
		m_timeElapsed = 0;

#ifdef _WIN32
		if (m_frequency == 0)
		{
			LARGE_INTEGER frequency;
			QueryPerformanceFrequency(&frequency);
			m_frequency = frequency.QuadPart;
		}
#elif defined NXNA_PLATFORM_APPLE
		if (m_info.denom == 0 ) {
			mach_timebase_info(&m_info);
		}
#endif
	}

	void StopWatch::Start()
	{
		if (m_running == false)
		{
			m_running = true;
			m_timeStarted = GetCurrentTicks();
		}
	}

	void StopWatch::Stop()
	{
		if (m_running == true)
		{
			m_running = false;
			m_timeElapsed += GetCurrentTicks() - m_timeStarted;
		}
	}

	void StopWatch::Reset()
	{
		m_running = false;
		m_timeElapsed = 0;
	}

	uint64_t StopWatch::GetElapsedTicks()
	{
		if (m_running == false)
			return m_timeElapsed;
		
		return GetCurrentTicks() - m_timeStarted;
	}

	uint64_t StopWatch::GetElapsedMilliseconds()
	{
#ifdef _WIN32
		return GetElapsedTicks() * 1000 / m_frequency;
#elif defined NXNA_PLATFORM_APPLE
		return GetElapsedTicks() * m_info.numer / m_info.denom / 1000000;
#endif
	}

	unsigned int StopWatch::GetElapsedMilliseconds32()
	{
#ifdef _WIN32
		return (unsigned int)(GetElapsedTicks() * 1000 / m_frequency);
#elif defined NXNA_PLATFORM_APPLE
		return (unsigned int)(GetElapsedTicks() * m_info.numer / m_info.denom / 1000000);
#endif
	}
}
}