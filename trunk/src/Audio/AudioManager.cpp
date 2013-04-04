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
		(*((SLEngineItf)AudioManager::GetEngineInterface()))->CreateAudioPlayer((SLEngineItf)AudioManager::GetEngineInterface(), &playerObj, &src, &dst, sizeof(ids), ids, req);
		(*playerObj)->Realize(playerObj, SL_BOOLEAN_FALSE);
		m_handle = (void*)playerObj;

		SLPlayItf player;
		SLVolumeItf playerVol;
		(*playerObj)->GetInterface(playerObj, SL_IID_PLAY, &player);
		(*playerObj)->GetInterface(playerObj, SL_IID_VOLUME, &playerVol);

		(*player)->RegisterCallback(player, playcallback, this);
		(*player)->SetCallbackEventsMask(player, SL_PLAYEVENT_HEADATEND);
 
#ifdef TARGET_ANDROID
		SLAndroidSimpleBufferQueueItf playerBuffer;
		(*player_obj)->GetInterface( player_obj, SL_IID_ANDROIDSIMPLEBUFFERQUEUE, &playerBuffer);
#else
		SLBufferQueueItf playerBuffer;
		(*playerObj)->GetInterface(playerObj, SL_IID_BUFFERQUEUE, &playerBuffer);
#endif

		(*playerBuffer)->Enqueue(playerBuffer, buffer->Data, buffer->DataLength);

		m_playing = true;
		(*player)->SetPlayState(player, SL_PLAYSTATE_PLAYING);
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
		(*playerObj)->GetInterface(playerObj, SL_IID_PLAY, &player);

		(*player)->SetPlayState(player, SL_PLAYSTATE_PAUSED);
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
		(*playerObj)->GetInterface(playerObj, SL_IID_PLAY, &player);

		SLuint32 state;
		(*player)->GetPlayState(player, &state);

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
#if defined NXNA_AUDIOENGINE_OPENAL
		m_device = (void*)alcOpenDevice(nullptr);

		m_context = (void*)alcCreateContext((ALCdevice*)m_device, nullptr);
		alcMakeContextCurrent((ALCcontext*)m_context);

		for (int i = 0; i < MAX_SOURCES; i++)
		{
			m_sources[i].Source = new AudioSource();
			m_sources[i].Owner = nullptr;
		}

		alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);

#elif defined NXNA_AUDIOENGINE_OPENSL
		SLObjectItf engineObj;
		SLEngineItf engine;

		slCreateEngine(&engineObj, 0, nullptr, 0, nullptr, nullptr);
		(*engineObj)->Realize(engineObj, SL_BOOLEAN_FALSE);
		(*engineObj)->GetInterface(engineObj, SL_IID_ENGINE, &engine);
		m_engine = (void*)engineObj;
		m_engineInterface = (void*)engine;

		SLObjectItf outputMixObj;
 
		const SLInterfaceID ids[] = { SL_IID_VOLUME };
		const SLboolean req[] = { SL_BOOLEAN_FALSE };
		(*engine)->CreateOutputMix(engine, &outputMixObj, 1, ids, req);
		(*outputMixObj)->Realize(outputMixObj, SL_BOOLEAN_FALSE);
		m_outputMix = (void*)outputMixObj;
#endif
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
		(*((SLObjectItf)m_outputMix))->Destroy((SLObjectItf)m_outputMix);
		m_outputMix = nullptr;
 
		(*((SLObjectItf)m_engine))->Destroy((SLObjectItf)m_engine);
		m_engine = nullptr;
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