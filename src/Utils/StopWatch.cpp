#include "StopWatch.h"

#ifdef _WIN32
#include <Windows.h>
#endif

namespace Nxna
{
namespace Utils
{
	uint64_t StopWatch::m_frequency = 0;

	uint64_t StopWatch::GetCurrentTicks()
	{
#ifdef _WIN32
		LARGE_INTEGER ticks;
		QueryPerformanceCounter(&ticks);
		return (uint64_t)ticks.QuadPart;
#endif
	}

	StopWatch::StopWatch()
	{
		m_running = false;
		m_timeElapsed = 0;

		if (m_frequency == 0)
		{
#ifdef _WIN32
			LARGE_INTEGER frequency;
			QueryPerformanceFrequency(&frequency);
			m_frequency = frequency.QuadPart;
#endif
		}
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
		return GetElapsedTicks() * 1000 / m_frequency;
	}

	unsigned int StopWatch::GetElapsedMilliseconds32()
	{
		return (unsigned int)(GetElapsedTicks() * 1000 / m_frequency);
	}
}
}