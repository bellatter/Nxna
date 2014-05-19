#include <cassert>
#include <cstring>
#include "SoundEffect.h"
#include "AudioManager.h"
#include "AudioListener.h"
#include "AudioEmitter.h"
#include "ADPCM/ADPCMDecoder.h"
#include "../Content/ContentManager.h"
#include "../Content/FileStream.h"

namespace Nxna
{
namespace Audio
{
	SoundEffectInstance::SoundEffectInstance(SoundEffect* effect)
	{
		assert(effect != nullptr);

		m_isLooped = false;
		m_source = nullptr;
		m_gain = 1.0f;
		m_positioned = false;

		m_bufferHandle = effect->m_bufferHandle;
	}

	SoundEffectInstance::~SoundEffectInstance()
	{
		if (isSourceValid())
			AudioManager::ReleaseSource(m_source);
	}

	void SoundEffectInstance::Play()
	{
		if (isSourceValid() == false)
		{
			m_source = AudioManager::GetFreeSource(this);
			if (m_source == nullptr) return;

			m_source->SetPosition(m_positioned == false, m_cachedPosition);
			m_source->SetBuffer(m_bufferHandle);
			m_source->IsLooping(m_isLooped);
		}

		m_source->Play(m_gain, 1.0f, 1.0f);
	}

	void SoundEffectInstance::Stop()
	{
		if (isSourceValid() == false) return;

		m_source->Stop();

		AudioManager::ReleaseSource(m_source);
		m_source = nullptr;
	}

	void SoundEffectInstance::Pause()
	{
		if (isSourceValid() == false) return;

		m_source->Pause();
	}

	bool SoundEffectInstance::IsLooped()
	{
		return m_isLooped;
	}
	
	void SoundEffectInstance::IsLooped(bool looped)
	{
		m_isLooped = looped;

		if (isSourceValid())
			m_source->IsLooping(looped);
	}

	float SoundEffectInstance::Volume()
	{
		return m_gain;
	}

	void SoundEffectInstance::Volume(float volume)
	{
		m_gain = volume;

		if (isSourceValid())
			m_source->SetGain(m_gain);
	}

	SoundState SoundEffectInstance::GetState()
	{
		if (isSourceValid())
		{
			return m_source->GetState();
		}

		return SoundState::Stopped;
	}

	void SoundEffectInstance::Apply3D(const AudioListener* listener, const AudioEmitter* emitter)
	{
		if (isSourceValid())
			AudioManager::Apply3D(m_source, listener, emitter);
		else
		{
			m_positioned = true;
			m_cachedPosition = emitter->GetPosition();
		}
			
/*
#ifndef DISABLE_OPENAL
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
		if (isSourceValid())
		{
			alSourcei(m_source, AL_SOURCE_RELATIVE, 0);
			alSource3f(m_source, AL_POSITION, emitter->GetPosition().X, emitter->GetPosition().Y, emitter->GetPosition().Z);
		}
		else
		{
			m_positioned = true;
			m_cachedPosition = emitter->GetPosition();
		}
#endif*/
	}

	bool SoundEffectInstance::isSourceValid()
	{
		return m_source != nullptr && AudioManager::IsSourceOwner(m_source, this);
	}


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

	std::vector<byte> SoundEffect::m_workingData;

	bool SoundEffect::Play()
	{
		return Play(1.0f, 0, 0);
	}

	bool SoundEffect::Play(float volume, float pitch, float pan)
	{
		AudioSource* source = AudioManager::GetFreeSource(nullptr);
		if (source == nullptr) return false;

		source->IsLooping(false);
		source->SetBuffer(m_bufferHandle);
		source->Play(volume, pitch, pan);

		return true;
	}

	SoundEffectInstance* SoundEffect::CreateInstance()
	{
		return new SoundEffectInstance(this);
	}

	void SoundEffect::SetDistanceScale(float scale)
	{
		AudioManager::SetDistanceScale(scale);
	}

	void SoundEffect::SetMasterVolume(float volume)
	{
		AudioManager::SetMasterVolume(volume);
	}

	SoundEffect* SoundEffect::LoadFrom(Content::Stream* stream)
	{
#ifndef DISABLE_OPENAL

		if (m_workingData.empty())
			m_workingData.resize(100 * 1024); // 100 KB

		int formatSize = stream->ReadInt32();
		if (formatSize < 18)
			throw Nxna::Content::ContentException("Sound Effect is in an unrecognized format.");

		WAVEFORMATEX format;
		stream->Read((byte*)&format, 18);

		int samplesPerBlock = 0;
		if (format.FormatTag == WAVE_FORMAT_ADPCM)
		{
			WAVEFORMATADPCM format2;
			stream->Read((byte*)(&format2.SamplesPerBlock), 4);
			format2.wf = format;

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

		if (format.FormatTag == WAVE_FORMAT_ADPCM)
		{
			AdpcmDecoder decoder;
			decoder.Decode(stream, format.Channels == 2, format.BitsPerSample, format.BlockAlign, samplesPerBlock);

			const byte* output;
			decoder.GetOutput(&output, &dataSize);

			if ((unsigned int)dataSize > m_workingData.capacity())
			{
				m_workingData.reserve(dataSize * 2);
			}

			memcpy(&m_workingData.front(), output, dataSize);
		}
		else
		{
			//dataSize = stream->ReadInt32();
			if (m_workingData.capacity() < (unsigned int)dataSize)
			{
				m_workingData.reserve(dataSize * 2);
			}
			stream->Read(&m_workingData.front(), dataSize);
		}

		SoundEffect* effect = new SoundEffect();
		AudioBuffer dummy;
		AudioBuffer::Create(format.Channels, format.BitsPerSample, format.SamplesPerSec, &m_workingData.front(), dataSize, &dummy);
		effect->m_bufferHandle = dummy.Handle;

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

