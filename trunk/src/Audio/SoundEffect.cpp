#include <cassert>
#include <cstring>
#include "SoundEffect.h"
#include "AudioManager.h"
#include "AudioListener.h"
#include "AudioEmitter.h"
#include "ADPCM/ADPCMDecoder.h"
#include "../Content/ContentManager.h"
#include "../Content/FileStream.h"
#include "../Utils/UnstableList.h"
#include "../MemoryAllocator.h"

#ifdef NXNA_AUDIOENGINE_OPENAL
#ifdef __APPLE__
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#elif defined NXNA_PLATFORM_NIX
#include <AL/al.h>
#include <AL/alc.h>
#else
#include <al.h>
#include <alc.h>
#endif
#endif

namespace Nxna
{
namespace Audio
{
	SoundEffectInstance::SoundEffectInstance(SoundEffect* effect)
	{
		assert(effect != nullptr);

		m_isLooped = false;
		m_gain = 1.0f;
		m_positioned = false;
		m_isFireAndForget = false;

#ifdef NXNA_AUDIOENGINE_OPENAL
		alGenSources(1, (ALuint*)&m_source);
#endif

		updateParent(effect);
	}

	SoundEffectInstance::~SoundEffectInstance()
	{
#ifdef NXNA_AUDIOENGINE_OPENAL
		alDeleteSources(1, (ALuint*)&m_source);
#endif
	}

	void SoundEffectInstance::Play()
	{
#ifdef NXNA_AUDIOENGINE_OPENAL
		alSourcePlay(m_source);
#endif
	}

	void SoundEffectInstance::Stop()
	{
#ifdef NXNA_AUDIOENGINE_OPENAL
		alSourceStop(m_source);
#endif
	}

	void SoundEffectInstance::Pause()
	{
#ifdef NXNA_AUDIOENGINE_OPENAL
		alSourcePause(m_source);
#endif
	}

	bool SoundEffectInstance::IsLooped()
	{
#ifdef NXNA_AUDIOENGINE_OPENAL
		ALint value;
		alGetSourcei(m_source, AL_LOOPING, &value);

		return value != 0;
#endif
	}
	
	void SoundEffectInstance::IsLooped(bool looped)
	{
#ifdef NXNA_AUDIOENGINE_OPENAL
		alSourcei(m_source, AL_LOOPING, looped ? AL_TRUE : AL_FALSE);
#endif
	}

	float SoundEffectInstance::Volume()
	{
#ifdef NXNA_AUDIOENGINE_OPENAL
		float gain;
		alGetSourcef((ALuint)m_source, AL_GAIN, &gain);
		return gain;
#endif
	}

	void SoundEffectInstance::Volume(float volume)
	{
#ifdef NXNA_AUDIOENGINE_OPENAL
		alSourcef((ALuint)m_source, AL_GAIN, volume);
#endif
	}

	SoundState SoundEffectInstance::GetState()
	{
#if defined NXNA_AUDIOENGINE_OPENAL
		int state;
		alGetSourcei((ALuint)m_source, AL_SOURCE_STATE, &state);

		if (state == AL_PLAYING)
			return SoundState::Playing;
		if (state == AL_PAUSED)
			return SoundState::Paused;

		return SoundState::Stopped;
#endif
	}

	void SoundEffectInstance::Apply3D(const AudioListener* listener, const AudioEmitter* emitter)
	{
#if defined NXNA_AUDIOENGINE_OPENAL
		// assume the OpenAL listener stays at (0,0,0)
		Nxna::Vector3 toEmitter = emitter->GetPosition() - listener->GetPosition();

		if (listener->m_orientationDirty == true)
		{
			listener->m_orientation = Nxna::Matrix::CreateWorld(Nxna::Vector3::Zero, listener->m_forward, listener->m_up);
			listener->m_orientationDirty = false;
		}

		Nxna::Vector3 finalPosition = Nxna::Vector3::Transform(toEmitter, listener->m_orientation);

		alSource3f(m_source, AL_POSITION, finalPosition.X, finalPosition.Y, finalPosition.Z);

#endif
	}

	void SoundEffectInstance::updateParent(SoundEffect* parent)
	{
		m_parent = parent;

#ifdef NXNA_AUDIOENGINE_OPENAL
		alSourcei(m_source, AL_BUFFER, m_parent->m_buffer);
#endif
	}

#ifdef NXNA_AUDIOENGINE_OPENAL
	struct WAVEFORMATEX
	{
		short FormatTag;
		short Channels;
		int SamplesPerSec;
		int AvgBytesPerSec;
		short BlockAlign;
		short BitsPerSample;
		short Size;
	};

	struct ADPCMCoefficient
	{
		int Coefficient1;
		int Coefficient2;
	};

	struct WAVEFORMATADPCM
	{
		WAVEFORMATEX wf;
		short SamplesPerBlock;
		short NumCoefficients;
	};

	const int WAVE_FORMAT_PCM = 1;
	const int WAVE_FORMAT_ADPCM = 2;
#endif

	std::vector<byte> SoundEffect::m_workingData;
	std::vector<SoundEffectInstance*> SoundEffect::m_instancePool;

	SoundEffect::~SoundEffect()
	{
		for (size_t i = 0; i < m_children.size(); i++)
		{
			m_children[i]->Stop();

			if (m_children[i]->m_isFireAndForget)
			{
				// return the instance to the queue
				m_instancePool.push_back(m_children[i]);
			}
			else
			{
				// unlink (we don't own the pointer, so it's someone else's job to delete the instance)
				m_children[i]->m_parent = nullptr;
			}
		}

#if defined NXNA_AUDIOENGINE_OPENAL
		alDeleteBuffers(1, (ALuint*)&m_buffer);
#endif
	}

	bool SoundEffect::Play()
	{
		return Play(1.0f, 0, 0);
	}

	bool SoundEffect::Play(float volume, float pitch, float pan)
	{
		SoundEffectInstance* instance;
		if (m_instancePool.empty() == false)
		{
			instance = m_instancePool.back();
			m_instancePool.pop_back();
			instance->updateParent(this);
		}
		else
		{
			instance = new SoundEffectInstance(this);
			instance->m_isFireAndForget = true;
		}

		m_children.push_back(instance);

		instance->IsLooped(false);
		instance->Volume(volume);
		instance->Play();

		return true;
	}

	SoundEffectInstance* SoundEffect::CreateInstance()
	{
		auto instance = new SoundEffectInstance(this);
		m_children.push_back(instance);
		return instance;
	}

	void SoundEffect::DestroyInstance(SoundEffectInstance* instance)
	{
		if (instance->m_isFireAndForget)
			throw new InvalidOperationException("Cannot destroy a SoundEffectInstance not created with SoundEffect::CreateInstance()");

		if (instance->m_parent != nullptr)
		{
			Utils::UnstableList<SoundEffectInstance*>::Remove(instance, instance->m_parent->m_children);
		}

		delete instance;
	}

	void SoundEffect::SetDistanceScale(float scale)
	{
#ifdef NXNA_AUDIOENGINE_OPENAL
		// TODO
#endif
	}

	void SoundEffect::SetMasterVolume(float volume)
	{
#ifdef NXNA_AUDIOENGINE_OPENAL
		alListenerf(AL_GAIN, volume);
#endif
	}

	SoundEffect* SoundEffect::LoadFrom(Content::Stream* stream)
	{
#ifndef DISABLE_OPENAL
		SoundEffect* effect = new SoundEffect();

		if (m_workingData.empty())
			m_workingData.resize(100 * 1024); // 100 KB

		int formatSize = stream->ReadInt32();
		if (formatSize < 18)
			throw Nxna::Content::ContentException("Sound Effect is in an unrecognized format.");

		WAVEFORMATEX format;
		stream->Read((byte*)&format, 18);

		short samplesPerBlock = 0;
		if (format.FormatTag == WAVE_FORMAT_ADPCM)
		{
			WAVEFORMATADPCM format2;
			stream->Read((byte*)(&format2.SamplesPerBlock), 2);
			format2.wf = format;

			// skip the # of coefficients
			stream->Seek(2, Content::SeekOrigin::Current);

			// skip the coefficients
			stream->Seek(7 * 4, Content::SeekOrigin::Current);

			samplesPerBlock = format2.SamplesPerBlock;
		}

		// validate the format of the sound
		if (format.FormatTag != WAVE_FORMAT_PCM && format.FormatTag != WAVE_FORMAT_ADPCM)
			throw Nxna::Content::ContentException("Sound Effect is in an unrecognized format.");

		if (format.Channels != 1 && format.Channels != 2)
			throw Nxna::Content::ContentException("Only mono and stereo sound effects are supported.");

		if (format.FormatTag == WAVE_FORMAT_PCM)
		{
			if ((format.BitsPerSample != 8 && format.BitsPerSample != 16) ||
				(format.SamplesPerSec != 22050 && format.SamplesPerSec != 44100 && format.SamplesPerSec != 48000))
				throw Nxna::Content::ContentException("Sound Effect is not in a supported format.");
		}
		else
		{
			if (format.BitsPerSample != 4)
				throw Nxna::Content::ContentException("Sound Effect is not in a supported format.");
		}

		int dataSize = stream->ReadInt32();

#ifdef NXNA_AUDIOENGINE_OPENAL
		ALenum bformat;
		if (format.Channels == 1)
		{
			if (format.BitsPerSample == 8)
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
			if (format.BitsPerSample == 8)
			{
				bformat = AL_FORMAT_STEREO8;
			}
			else
			{
				bformat = AL_FORMAT_STEREO16;
			}
		}

		byte* buffer = nullptr;

		if (format.FormatTag == WAVE_FORMAT_ADPCM)
		{
			AdpcmDecoder decoder(stream, format.Channels == 2, format.BitsPerSample, format.BlockAlign, samplesPerBlock);
			buffer = (byte*)NxnaTempMemoryPool::GetMemory(decoder.GetRequiredBufferSize());
			decoder.Decode(buffer);
		}
		else
		{
			buffer = (byte*)Nxna::NxnaTempMemoryPool::GetMemory(dataSize);
			stream->Read(buffer, dataSize);
		}

		alGenBuffers(1, (ALuint*)&effect->m_buffer);
		alBufferData((ALuint)effect->m_buffer, bformat, buffer, dataSize, format.SamplesPerSec);

		Nxna::NxnaTempMemoryPool::ReleaseMemory();
#endif



		return effect;
#else
		return new SoundEffect();
#endif
	}

	void* SoundEffectLoader::Read(Content::XnbReader* stream)
	{
		stream->ReadTypeID();

		return SoundEffect::LoadFrom(stream->GetStream());
	}

	void SoundEffectLoader::Destroy(void* resource)
	{
		delete static_cast<SoundEffect*>(resource);
	}
}
}

