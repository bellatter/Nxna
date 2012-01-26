#ifndef DISABLE_OPENAL
#ifdef __APPLE__
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else
#include <al.h>
#include <alc.h>
#endif
#endif
#include <cassert>
#include "AudioManager.h"

namespace Nxna
{
namespace Audio
{
	void* AudioManager::m_device = nullptr;
	void* AudioManager::m_context = nullptr;
	AudioManager::SourceInfo AudioManager::m_sources[MAX_SOURCES];

	void AudioManager::Init()
	{
#ifndef DISABLE_OPENAL
		m_device = (void*)alcOpenDevice(nullptr);

		m_context = (void*)alcCreateContext((ALCdevice*)m_device, nullptr);
		alcMakeContextCurrent((ALCcontext*)m_context);

		for (int i = 0; i < MAX_SOURCES; i++)
		{
			alGenSources(1, &m_sources[i].Source);
			m_sources[i].Reserved = false;
		}
#endif
	}

	void AudioManager::Shutdown()
	{
#ifndef DISABLE_OPENAL
		alcMakeContextCurrent(nullptr);
		alcDestroyContext((ALCcontext*)m_context);
		alcCloseDevice((ALCdevice*)m_device);
#endif
	}

	AudioSource AudioManager::GetFreeSource(bool reserve)
	{
#ifndef DISABLE_OPENAL
		for (int i = 0; i < MAX_SOURCES; i++)
		{
			if (m_sources[i].Reserved == false)
			{
				int state;
				alGetSourcei(m_sources[i].Source, AL_SOURCE_STATE, &state);
				if (state == AL_STOPPED || state == AL_INITIAL)
				{
					m_sources[i].Reserved = reserve;
					return m_sources[i].Source;
				}
			}
		}
#endif

		return -1;
	}

	void AudioManager::ReleaseSource(AudioSource source)
	{
#ifndef DISABLE_OPENAL
		for (int i = 0; i < MAX_SOURCES; i++)
		{
			if (m_sources[i].Source == source)
			{
				m_sources[i].Reserved = false;
				return;
			}
		}
#endif
	}
}
}