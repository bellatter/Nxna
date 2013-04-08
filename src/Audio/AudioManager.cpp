#include "../NxnaConfig.h"

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
#include "../Logger.h"

#ifdef NXNA_AUDIOENGINE_OPENSL
#include "OpenSLES.h"
#endif

#include <memory.h>

// NOTE: Most of the OpenSL code is based on http://vec3.ca/getting-started-with-opensl-on-android/

namespace Nxna
{
namespace Audio
{
	void AudioBuffer::Create(int numChannels, int numBitsPerSample, int numSamplesPerSecond, byte* data, int numBytes, AudioBuffer* result)
	{
#if defined NXNA_AUDIOENGINE_OPENAL
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
#elif defined NXNA_AUDIOENGINE_OPENSL
		result->Handle = result;
		result->NumBitsPerSample = numBitsPerSample;
		result->NumChannels = numChannels;
		result->NumSamplesPerSecond = numSamplesPerSecond;
		result->Data = new byte[numBytes];
		memcpy(result->Data, data, numBytes);
		result->DataLength = numBytes;
#endif
	}

	AudioSource::AudioSource()
	{
		m_handle = nullptr;
		m_bufferHandle = nullptr;

#if defined NXNA_AUDIOENGINE_OPENAL
		alGenSources(1, (ALuint*)&m_handle);
#elif defined NXNA_AUDIOENGINE_OPENSL
		m_playing = false;
#endif
	}

	AudioSource::~AudioSource()
	{
#if defined NXNA_AUDIOENGINE_OPENAL
		alDeleteSources(1, (ALuint*)&m_handle);
#elif defined NXNA_AUDIOENGINE_OPENSL
		if (m_handle != nullptr)
			(*(SLObjectItf)m_handle)->Destroy((SLObjectItf)m_handle);
#endif
	}

	bool AudioSource::IsAvailable()
	{
#if defined NXNA_AUDIOENGINE_OPENAL
		int state;
		alGetSourcei((ALuint)m_handle, AL_SOURCE_STATE, &state);
		return state == AL_STOPPED || state == AL_INITIAL;
#elif defined NXNA_AUDIOENGINE_OPENSL
		return m_handle == nullptr || !m_playing;
#else
		return false;
#endif
	}

	void AudioSource::SetBuffer(void* buffer)
	{
		m_bufferHandle = buffer;
	}

#if defined NXNA_AUDIOENGINE_OPENSL
	void SLAPIENTRY playcallback(SLPlayItf player, void* context, SLuint32 e)
	{
		if (e & SL_PLAYEVENT_HEADATEND)
		{
			((AudioSource*)context)->OnStop();
		}
	}
#endif

	void AudioSource::Play(float volume, float pitch, float pan)
	{
#if defined NXNA_AUDIOENGINE_OPENAL
		alSourcei((ALuint)m_handle, AL_SOURCE_RELATIVE, 1);
		alSource3f((ALuint)m_handle, AL_POSITION, 0, 0, 0);
		alSourcei((ALuint)m_handle, AL_LOOPING, 0);
		alSourcei((ALuint)m_handle, AL_BUFFER, (ALint)m_bufferHandle);
		alSourcef((ALuint)m_handle, AL_GAIN, volume);
		alSourcef((ALuint)m_handle, AL_REFERENCE_DISTANCE, AudioManager::m_distanceScale);
		
		// TODO: what about "pitch" and "pan"?

		alSourcePlay((ALuint)m_handle);
#elif defined NXNA_AUDIOENGINE_OPENSL

		SLEngineItf engine = (SLEngineItf)AudioManager::GetEngineInterface();
		if (engine == nullptr) 
			return;

		if (m_bufferHandle == nullptr)
			return;

		if (m_handle != nullptr)
		{
			(*(SLObjectItf)m_handle)->Destroy((SLObjectItf)m_handle);
		}

		AudioBuffer* buffer = (AudioBuffer*)m_bufferHandle;
		SLDataFormat_PCM format;
		format.formatType = SL_DATAFORMAT_PCM;
		format.numChannels = buffer->NumChannels;
		format.samplesPerSec = buffer->NumSamplesPerSecond * 1000; // this is actually millihertz, not hertz
		format.bitsPerSample = (buffer->NumBitsPerSample == 8 ? SL_PCMSAMPLEFORMAT_FIXED_8 : SL_PCMSAMPLEFORMAT_FIXED_16);
		format.containerSize = buffer->NumBitsPerSample;
		format.channelMask = SL_SPEAKER_FRONT_CENTER;
		format.endianness = SL_BYTEORDER_LITTLEENDIAN;

#ifdef NXNA_PLATFORM_ANDROID
		SLDataLocator_AndroidSimpleBufferQueue in_loc;
		in_loc.locatorType = SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE;
		in_loc.numBuffers = 1;
#else
		SLDataLocator_BufferQueue in_loc;
		in_loc.locatorType = SL_DATALOCATOR_BUFFERQUEUE;
		in_loc.numBuffers = 1;
#endif

		SLDataSource src;
		src.pFormat = &format;
		src.pLocator = &in_loc;

		SLDataLocator_OutputMix out_loc;
		out_loc.locatorType = SL_DATALOCATOR_OUTPUTMIX;
		out_loc.outputMix = (SLObjectItf)AudioManager::GetOutputMix();
 
		SLDataSink dst;
		dst.pLocator = &out_loc;
		dst.pFormat = nullptr;

#ifdef TARGET_ANDROID
const SLInterfaceID ids[] = { SL_IID_VOLUME, SL_IID_ANDROIDSIMPLEBUFFERQUEUE };
const SLboolean req[] = { SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE };
#else
const SLInterfaceID ids[] = { SL_IID_VOLUME };
const SLboolean req[] = { SL_BOOLEAN_TRUE };
#endif

		SLObjectItf playerObj;
		SLresult r = (*engine)->CreateAudioPlayer(engine, &playerObj, &src, &dst, sizeof(ids), ids, req);
		if (r != SL_RESULT_SUCCESS)
		{
			NXNA_LOG_DEBUG("Unable to create OpenSL audio player (error: %u)", r);
			m_handle = nullptr;
			m_playing = false;
			return;
		}
		r = (*playerObj)->Realize(playerObj, SL_BOOLEAN_FALSE);
		if (r != SL_RESULT_SUCCESS)
		{
			NXNA_LOG_DEBUG("Unable to realize OpenSL audio player (error: %u)", r);
			m_handle = nullptr;
			m_playing = false;
			return;
		}
		m_handle = (void*)playerObj;
		m_playing = true;

		SLPlayItf player;
		SLVolumeItf playerVol;
		r = (*playerObj)->GetInterface(playerObj, SL_IID_PLAY, &player);
		if (r != SL_RESULT_SUCCESS)
		{
			NXNA_LOG_DEBUG("Unable to get OpenSL audio player interface (error: %u)", r);
			(*playerObj)->Destroy(playerObj);
			m_handle = nullptr;
			m_playing = false;
			return;
		}
		r = (*playerObj)->GetInterface(playerObj, SL_IID_VOLUME, &playerVol);
		if (r != SL_RESULT_SUCCESS)
		{
			NXNA_LOG_DEBUG("Unable to get OpenSL audio player volume interface (error: %u)", r);
			(*playerObj)->Destroy(playerObj);
			m_handle = nullptr;
			m_playing = false;
			return;
		}

		r = (*player)->RegisterCallback(player, playcallback, this);
		if (r != SL_RESULT_SUCCESS)
		{
			NXNA_LOG_DEBUG("Unable to register OpenSL audio player callback (error: %u)", r);
			(*playerObj)->Destroy(playerObj);
			m_handle = nullptr;
			m_playing = false;
			return;
		}
		r = (*player)->SetCallbackEventsMask(player, SL_PLAYEVENT_HEADATEND);
		if (r != SL_RESULT_SUCCESS)
		{
			NXNA_LOG_DEBUG("Unable to set OpenSL audio player callback mask (error: %u)", r);
			(*playerObj)->Destroy(playerObj);
			m_handle = nullptr;
			m_playing = false;
			return;
		}
 
#ifdef TARGET_ANDROID
		SLAndroidSimpleBufferQueueItf playerBuffer;
		r = (*player_obj)->GetInterface( player_obj, SL_IID_ANDROIDSIMPLEBUFFERQUEUE, &playerBuffer);
#else
		SLBufferQueueItf playerBuffer;
		r = (*playerObj)->GetInterface(playerObj, SL_IID_BUFFERQUEUE, &playerBuffer);
#endif
		if (r != SL_RESULT_SUCCESS)
		{
			NXNA_LOG_DEBUG("Unable to get OpenSL audio player buffer queue interface (error: %u)", r);
			(*playerObj)->Destroy(playerObj);
			m_handle = nullptr;
			m_playing = false;
			return;
		}

		r = (*playerBuffer)->Enqueue(playerBuffer, buffer->Data, buffer->DataLength);
		if (r != SL_RESULT_SUCCESS)
		{
			NXNA_LOG_DEBUG("Unable to queue OpenSL audio player buffer (error: %u)", r);
			(*playerObj)->Destroy(playerObj);
			m_handle = nullptr;
			m_playing = false;
			return;
		}

		r = (*player)->SetPlayState(player, SL_PLAYSTATE_PLAYING);
		if (r != SL_RESULT_SUCCESS)
		{
			NXNA_LOG_DEBUG("Unable to play OpenSL audio player (error: %u)", r);
			(*playerObj)->Destroy(playerObj);
			m_handle = nullptr;
			m_playing = false;
			return;
		}
#endif
	}

	void AudioSource::Pause()
	{
#if defined NXNA_AUDIOENGINE_OPENAL
		alSourcePause((ALuint)m_handle);
#elif defined NXNA_AUDIOENGINE_OPENSL
		if (m_handle == nullptr || m_playing == false) return;

		SLObjectItf playerObj = (SLObjectItf)m_handle;

		SLPlayItf player;
		SLresult r = (*playerObj)->GetInterface(playerObj, SL_IID_PLAY, &player);
		if (r != SL_RESULT_SUCCESS)
		{
			NXNA_LOG_DEBUG("Unable to get OpenSL audio player interface (error: %u)", r);
			return;
		}

		r = (*player)->SetPlayState(player, SL_PLAYSTATE_PAUSED);
		if (r != SL_RESULT_SUCCESS)
		{
			NXNA_LOG_DEBUG("Unable to pause OpenSL audio player (error: %u)", r);
			return;
		}
#endif
	}

	void AudioSource::Stop()
	{
#ifdef NXNA_AUDIOENGINE_OPENAL
		alSourceStop((ALuint)m_handle);
#elif defined NXNA_AUDIOENGINE_OPENSL
		if (m_handle == nullptr || m_playing == false) return;

		SLObjectItf playerObj = (SLObjectItf)m_handle;
		(*playerObj)->Destroy(playerObj);

		m_handle = nullptr;
		m_playing = false;
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
#if defined NXNA_AUDIOENGINE_OPENAL
		int state;
		alGetSourcei((ALuint)m_handle, AL_SOURCE_STATE, &state);

		if (state == AL_PLAYING)
			return SoundState::Playing;
		if (state == AL_PAUSED)
			return SoundState::Paused;
#elif defined NXNA_AUDIOENGINE_OPENSL
		if (m_handle == nullptr || m_playing == false)
			return SoundState::Stopped;

		SLObjectItf playerObj = (SLObjectItf)m_handle;
		SLPlayItf player;
		SLresult r = (*playerObj)->GetInterface(playerObj, SL_IID_PLAY, &player);
		if (r != SL_RESULT_SUCCESS)
		{
			NXNA_LOG_DEBUG("Unable to get OpenSL audio player interface (error: %u)", r);
			return SoundState::Stopped;
		}

		SLuint32 state;
		r = (*player)->GetPlayState(player, &state);
		if (r != SL_RESULT_SUCCESS)
		{
			NXNA_LOG_DEBUG("Unable to get OpenSL audio player state (error: %u)", r);
			return SoundState::Stopped;
		}

		if (state == SL_PLAYSTATE_PLAYING)
			return SoundState::Playing;
		if (state == SL_PLAYSTATE_PAUSED)
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

#if defined NXNA_AUDIOENGINE_OPENAL
	void* AudioManager::m_device = nullptr;
	void* AudioManager::m_context = nullptr;
#elif defined NXNA_AUDIOENGINE_OPENSL
	void* AudioManager::m_engine = nullptr;
	void* AudioManager::m_outputMix = nullptr;
	void* AudioManager::m_engineInterface = nullptr;
#endif
	AudioManager::SourceInfo AudioManager::m_sources[MAX_SOURCES];
	float AudioManager::m_distanceScale = 1.0f;

	void AudioManager::Init()
	{
		NXNA_LOG_DEBUG("Initializing audio...");

#if defined NXNA_AUDIOENGINE_OPENAL
		m_device = (void*)alcOpenDevice(nullptr);

		m_context = (void*)alcCreateContext((ALCdevice*)m_device, nullptr);
		alcMakeContextCurrent((ALCcontext*)m_context);

		alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);

#elif defined NXNA_AUDIOENGINE_OPENSL
		SLObjectItf engineObj;
		SLEngineItf engine;

		SLresult r = slCreateEngine(&engineObj, 0, nullptr, 0, nullptr, nullptr);
		if (r != SL_RESULT_SUCCESS)
		{
			NXNA_LOG_DEBUG("Unable to create OpenSL engine (error: %u)", r);
			return;
		}
		r = (*engineObj)->Realize(engineObj, SL_BOOLEAN_FALSE);
		if (r != SL_RESULT_SUCCESS)
		{
			NXNA_LOG_DEBUG("Unable to realize OpenSL engine (error: %u)", r);
			(*engineObj)->Destroy(engineObj);
			return;
		}
		r = (*engineObj)->GetInterface(engineObj, SL_IID_ENGINE, &engine);
		if (r != SL_RESULT_SUCCESS)
		{
			NXNA_LOG_DEBUG("Unable to get OpenSL engine interface (error: %u)", r);
			(*engineObj)->Destroy(engineObj);
			return;
		}
		m_engine = (void*)engineObj;
		m_engineInterface = (void*)engine;

		SLObjectItf outputMixObj;
 
		const SLInterfaceID ids[] = { SL_IID_VOLUME };
		const SLboolean req[] = { SL_BOOLEAN_FALSE };
		r = (*engine)->CreateOutputMix(engine, &outputMixObj, 1, ids, req);
		if (r != SL_RESULT_SUCCESS)
		{
			NXNA_LOG_DEBUG("Unable to create OpenSL output mix (error: %u)", r);
			(*engineObj)->Destroy(engineObj);
			m_engine = nullptr;
			m_engineInterface = nullptr;
		}
		r = (*outputMixObj)->Realize(outputMixObj, SL_BOOLEAN_FALSE);
		if (r != SL_RESULT_SUCCESS)
		{
			NXNA_LOG_DEBUG("Unable to realize OpenSL output mix (error: %u)", r);
			(*outputMixObj)->Destroy(outputMixObj);
			(*engineObj)->Destroy(engineObj);
			m_engine = nullptr;
			m_engineInterface = nullptr;
			return;
		}
		m_outputMix = (void*)outputMixObj;
#endif

		for (int i = 0; i < MAX_SOURCES; i++)
		{
			m_sources[i].Source = new AudioSource();
			m_sources[i].Owner = nullptr;
		}

		NXNA_LOG_DEBUG("Audio initialized");
	}

	void AudioManager::Shutdown()
	{
		for (int i = 0; i < MAX_SOURCES; i++)
		{
			delete m_sources[i].Source;
			m_sources[i].Source = nullptr;
			m_sources[i].Owner = nullptr;
		}

#if defined NXNA_AUDIOENGINE_OPENAL
		alcMakeContextCurrent(nullptr);
		alcDestroyContext((ALCcontext*)m_context);
		alcCloseDevice((ALCdevice*)m_device);

#elif defined NXNA_AUDIOENGINE_OPENSL
		if (m_outputMix != nullptr)
		{
			(*((SLObjectItf)m_outputMix))->Destroy((SLObjectItf)m_outputMix);
			m_outputMix = nullptr;
		}
 
		if (m_engine != nullptr)
		{
			(*((SLObjectItf)m_engine))->Destroy((SLObjectItf)m_engine);
			m_engine = nullptr;
		}
#endif
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