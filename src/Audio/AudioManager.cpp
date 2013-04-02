#ifdef NXNA_AUDIOENGINE_OPENAL
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
#include "AudioListener.h"
#include "AudioEmitter.h"

#ifdef NXNA_AUDIOENGINE_OPENSL
#include "OpenSLES.h"
#endif

namespace Nxna
{
namespace Audio
{
	void AudioBuffer::Create(int numChannels, int numBitsPerSample, int numSamplesPerSecond, byte* data, int numBytes, AudioBuffer* result)
	{
#ifdef NXNA_AUDIOENGINE_OPENAL
		ALenum bformat;
		if (numChannels == 1)
		{
			if (numBitsPerSample == 8)
			{
				bformat = AL_FORMAT_MONO8;
			}
			else
			{
				bformat = AL_FORMAT_MONO16;
			}
		}
		else
		{
			if (numBitsPerSample == 8)
			{
				bformat = AL_FORMAT_STEREO8;
			}
			else
			{
				bformat = AL_FORMAT_STEREO16;
			}
		}

		alGenBuffers(1, (ALuint*)&result->Handle);
		alBufferData((ALuint)result->Handle, bformat, data, numBytes, numSamplesPerSecond);
#endif
	}

	AudioSource::AudioSource()
	{
		m_handle = nullptr;
		m_bufferHandle = nullptr;

#ifdef NXNA_AUDIOENGINE_OPENAL
		alGenSources(1, (ALuint*)&m_handle);
#endif
	}

	AudioSource::~AudioSource()
	{
#ifdef NXNA_AUDIOENGINE_OPENAL
		alDeleteSources(1, (ALuint*)&m_handle);
#endif
	}

	bool AudioSource::IsAvailable()
	{
#ifdef NXNA_AUDIOENGINE_OPENAL
		int state;
		alGetSourcei((ALuint)m_handle, AL_SOURCE_STATE, &state);
		return state == AL_STOPPED || state == AL_INITIAL;
#else
		return false;
#endif
	}

	void AudioSource::SetBuffer(void* buffer)
	{
		m_bufferHandle = buffer;
	}

	void AudioSource::Play(float volume, float pitch, float pan)
	{
#ifdef NXNA_AUDIOENGINE_OPENAL
		alSourcei((ALuint)m_handle, AL_SOURCE_RELATIVE, 1);
		alSource3f((ALuint)m_handle, AL_POSITION, 0, 0, 0);
		alSourcei((ALuint)m_handle, AL_LOOPING, 0);
		alSourcei((ALuint)m_handle, AL_BUFFER, (ALint)m_bufferHandle);
		alSourcef((ALuint)m_handle, AL_GAIN, volume);
		alSourcef((ALuint)m_handle, AL_REFERENCE_DISTANCE, AudioManager::m_distanceScale);
		
		// TODO: what about "pitch" and "pan"?

		alSourcePlay((ALuint)m_handle);
#endif
	}

	void AudioSource::Pause()
	{
#ifdef NXNA_AUDIOENGINE_OPENAL
		alSourcePause((ALuint)m_handle);
#endif
	}

	void AudioSource::Stop()
	{
#ifdef NXNA_AUDIOENGINE_OPENAL
		alSourceStop((ALuint)m_handle);
#endif
	}

	void AudioSource::SetGain(float gain)
	{
#ifdef NXNA_AUDIOENGINE_OPENAL
		alSourcef((ALuint)m_handle, AL_GAIN, gain);
#endif
	}

	SoundState AudioSource::GetState()
	{
#ifdef NXNA_AUDIOENGINE_OPENAL
		int state;
		alGetSourcei((ALuint)m_handle, AL_SOURCE_STATE, &state);

		if (state == AL_PLAYING)
			return SoundState::Playing;
		if (state == AL_PAUSED)
			return SoundState::Paused;
#endif

		return SoundState::Stopped;
	}

	void AudioSource::IsLooping(bool looping)
	{
#ifdef NXNA_AUDIOENGINE_OPENAL
		alSourcei((ALuint)m_handle, AL_LOOPING, looping ? 1 : 0);
#endif
	}

	void AudioSource::SetPosition(bool relative, const Vector3& position)
	{
#ifdef NXNA_AUDIOENGINE_OPENAL
		if (!relative)
		{
			alSourcei((ALuint)m_handle, AL_SOURCE_RELATIVE, 0);
			alSource3f((ALuint)m_handle, AL_POSITION, position.X, position.Y, position.Z);
		}
		else
		{
			alSourcei((ALuint)m_handle, AL_SOURCE_RELATIVE, 1);
			alSource3f((ALuint)m_handle, AL_POSITION, 0, 20, 0);
		}
#endif
	}

	void* AudioManager::m_device = nullptr;
	void* AudioManager::m_context = nullptr;
	AudioManager::SourceInfo AudioManager::m_sources[MAX_SOURCES];
	float AudioManager::m_distanceScale = 1.0f;

	void AudioManager::Init()
	{
#ifdef NXNA_AUDIOENGINE_OPENAL
		m_device = (void*)alcOpenDevice(nullptr);

		m_context = (void*)alcCreateContext((ALCdevice*)m_device, nullptr);
		alcMakeContextCurrent((ALCcontext*)m_context);

		for (int i = 0; i < MAX_SOURCES; i++)
		{
			m_sources[i].Source = new AudioSource();
			m_sources[i].Owner = nullptr;
		}

		alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);
#endif

		/*
		SLObjectItf engineObj;
		SLEngineItf engine;

		slCreateEngine(&engineObj, 0, nullptr, 0, nullptr, nullptr);
		(*engineObj)->Realize(engineObj, SL_BOOLEAN_FALSE);
		(*engineObj)->GetInterface(engineObj, SL_IID_ENGINE, &engine);

		SLObjectItf output_mix_obj;
 
		const SLInterfaceID ids[] = { SL_IID_VOLUME };
		const SLboolean req[] = { SL_BOOLEAN_FALSE };
		(*engine)->CreateOutputMix(engine, &output_mix_obj, 1, ids, req );

		*/
	}

	void AudioManager::Shutdown()
	{
		for (int i = 0; i < MAX_SOURCES; i++)
		{
			delete m_sources[i].Source;
			m_sources[i].Source = nullptr;
			m_sources[i].Owner = nullptr;
		}

#ifdef NXNA_AUDIOENGINE_OPENAL
		alcMakeContextCurrent(nullptr);
		alcDestroyContext((ALCcontext*)m_context);
		alcCloseDevice((ALCdevice*)m_device);
#endif

		/*
		(*output_mix_obj)->Destroy( output_mix_obj );
		output_mix_obj = nullptr;
		output_mix_vol = nullptr;
 
		(*engine_obj)->Destroy( engine_obj );
		engine_obj = nullptr;
		engine = nullptr;*/
	}

	AudioSource* AudioManager::GetFreeSource(void* owner)
	{
		for (int i = 0; i < MAX_SOURCES; i++)
		{
			if (m_sources[i].Source->IsAvailable())
			{
				m_sources[i].Owner = owner;
				return m_sources[i].Source;
			}
		}

		return nullptr;
	}

	void AudioManager::ReleaseSource(AudioSource* source)
	{
		for (int i = 0; i < MAX_SOURCES; i++)
		{
			if (m_sources[i].Source == source)
			{
				m_sources[i].Owner = nullptr;
				return;
			}
		}
	}

	bool AudioManager::IsSourceOwner(AudioSource* source, void* owner)
	{
		for (int i = 0; i < MAX_SOURCES; i++)
		{
			if (m_sources[i].Source == source)
			{
				return m_sources[i].Owner == owner;
			}
		}

		return false;
	}

	void AudioManager::SetDistanceScale(float scale)
	{
		m_distanceScale = scale;

#ifdef NXNA_AUDIOENGINE_OPENAL
		for (int i = 0; i < MAX_SOURCES; i++)
		{
			alSourcef((ALuint)m_sources[i].Source, AL_REFERENCE_DISTANCE, scale);
		}
#endif
	}

	void AudioManager::SetMasterVolume(float volume)
	{
#ifdef NXNA_AUDIOENGINE_OPENAL
		alListenerf(AL_GAIN, volume);
#endif
	}

	void AudioManager::Apply3D(AudioSource* source, const AudioListener* listener, const AudioEmitter* emitter)
	{
#ifdef NXNA_AUDIOENGINE_OPENAL
		// TODO: OpenAL doesn't support multiple listeners like XNA does,
		// but it may be possible to fake it...

		// update the listener
		Vector3 position = listener->GetPosition();
		Vector3 forward = listener->GetForward();
		Vector3 up = listener->GetUp();

		alListener3f(AL_POSITION, position.X, position.Y, position.Z);

		float forwardAndUp[] = {
			forward.X, forward.Y, forward.Z,
			up.X, up.Y, up.Z
		};
		alListenerfv(AL_ORIENTATION, forwardAndUp);

		// update the source
		source->SetPosition(false, emitter->GetPosition());
#endif
	}
}
}