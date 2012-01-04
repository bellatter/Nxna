#ifndef DISABLE_OPENAL
#ifdef __APPLE__
#include <OpenAL/al.h>
#else
#include <al.h>
#endif
#endif
#include <cassert>
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

#ifndef DISABLE_OPENAL
		m_source = AudioManager::GetFreeSource(true);

		alSourcei(m_source, AL_BUFFER, effect->m_alBuffer);
		alSourcei(m_source, AL_SOURCE_RELATIVE, 1);
		alSourcei(m_source, AL_LOOPING, 0);
		alSource3f(m_source, AL_POSITION, 0, 20, 0);
		alSourcef(m_source, AL_GAIN, 1.0f);
#endif
	}

	SoundEffectInstance::~SoundEffectInstance()
	{
		AudioManager::ReleaseSource(m_source);
	}

	void SoundEffectInstance::Play()
	{
#ifndef DISABLE_OPENAL
		alSourcePlay(m_source);
#endif
	}

	void SoundEffectInstance::Stop()
	{
#ifndef DISABLE_OPENAL
		alSourceStop(m_source);
#endif
	}

	bool SoundEffectInstance::IsLooped()
	{
#ifndef DISABLE_OPENAL
		int state;
		alGetSourcei(m_source, AL_LOOPING, &state);
		if (state == 0)
			return false;

		return true;
#else
		return false;
#endif
	}
	
	void SoundEffectInstance::IsLooped(bool looped)
	{
#ifndef DISABLE_OPENAL
		alSourcei(m_source, AL_LOOPING, looped ? 1 : 0);
#endif
	}

	float SoundEffectInstance::Volume()
	{
#ifndef DISABLE_OPENAL
		float gain;
		alGetSourcef(m_source, AL_GAIN, &gain);

		return gain;
#else
		return 0;
#endif
	}

	void SoundEffectInstance::Volume(float volume)
	{
#ifndef DISABLE_OPENAL
		alSourcef(m_source, AL_GAIN, volume);
#endif
	}

	SoundState SoundEffectInstance::GetState()
	{
#ifndef DISABLE_OPENAL
		int state;
		alGetSourcei(m_source, AL_SOURCE_STATE, &state);

		if (state == AL_PLAYING)
			return SoundState_Playing;
		if (state == AL_PAUSED)
			return SoundState_Paused;
#endif

		return SoundState_Stopped;
	}

	void SoundEffectInstance::Apply3D(const AudioListener* listener, const AudioEmitter* emitter)
	{
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
		alSourcei(m_source, AL_SOURCE_RELATIVE, 0);
		alSource3f(m_source, AL_POSITION, emitter->GetPosition().X, emitter->GetPosition().Y, emitter->GetPosition().Z);
#endif
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

	byte* SoundEffect::m_workingData = nullptr;
	int SoundEffect::m_workingDataLength = 0;

	bool SoundEffect::Play()
	{
		return Play(1.0f, 0, 0);
	}

	bool SoundEffect::Play(float volume, float pitch, float pan)
	{
#ifndef DISABLE_OPENAL
		AudioSource source = AudioManager::GetFreeSource(false);

		alSourcei(source, AL_SOURCE_RELATIVE, 1);
		alSourcei(source, AL_LOOPING, 0);
		alSourcei(source, AL_BUFFER, m_alBuffer);
		alSourcef(source, AL_GAIN, volume);
		
		// TODO: what about "pitch" and "pan"?

		alSourcePlay(source);
#endif
		return true;
	}

	SoundEffectInstance* SoundEffect::CreateInstance()
	{
		return new SoundEffectInstance(this);
	}

	SoundEffect* SoundEffect::LoadFrom(Content::FileStream* stream)
	{
#ifndef DISABLE_OPENAL
		int formatSize = stream->ReadInt32();
		if (formatSize < 18)
			throw Nxna::Content::ContentException("Sound Effect is in an unrecognized format.");

		WAVEFORMATEX format;
		stream->Read((byte*)&format, 18);

		int samplesPerBlock;
		if (format.FormatTag == WAVE_FORMAT_ADPCM)
		{
			WAVEFORMATADPCM format2;
			stream->Read((byte*)(&format2.SamplesPerBlock), 4);
			format2.wf = format;

			// skip the coefficients
			stream->Advance(7 * 4);

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
				(format.SamplesPerSec != 22050 && format.SamplesPerSec != 44100))
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

			byte* output;
			decoder.GetOutput(&output, &dataSize);

			if (dataSize > m_workingDataLength)
			{
				delete[] m_workingData;
				m_workingData = new byte[dataSize * 2];
				m_workingDataLength = dataSize * 2;
			}

			memcpy(m_workingData, output, dataSize);
		}
		else
		{
			//dataSize = stream->ReadInt32();
			if (m_workingDataLength < dataSize)
			{
				delete[] m_workingData;
				m_workingData = new byte[dataSize * 2];
				m_workingDataLength = dataSize * 2;
			}
			stream->Read(m_workingData, dataSize);
		}

		SoundEffect* effect = new SoundEffect();
		alGenBuffers(1, &effect->m_alBuffer);

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
		alBufferData(effect->m_alBuffer, bformat, m_workingData, dataSize, format.SamplesPerSec);

		effect->m_duration = (float)dataSize / format.SamplesPerSec / format.BitsPerSample / 8;

		return effect;
#else
		return new SoundEffect();
#endif
	}

	void* SoundEffectLoader::Load(Content::XnbReader* stream)
	{
		int typeID = stream->ReadTypeID();

		return SoundEffect::LoadFrom(stream->GetStream());
	}

	void SoundEffectLoader::Destroy(void* resource)
	{
		delete static_cast<SoundEffect*>(resource);
	}
}
}

