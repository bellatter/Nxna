#include "MediaPlayer.h"
#include "Song.h"
#include "../Audio/AudioManager.h"
#include "../Content/FileStream.h"
#include "../Audio/OggVorbis/OggVorbisDecoder.h"

#ifndef DISABLE_OPENAL
#ifdef __APPLE__
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else
#include <al.h>
#include <alc.h>
#endif
#endif

namespace Nxna
{
namespace Media
{
	Song* MediaPlayer::m_currentSong = nullptr;
	int MediaPlayer::m_source = 0;
	unsigned int MediaPlayer::m_buffers[];
	bool MediaPlayer::m_repeat = false;

	void MediaPlayer::Play(Song* song)
	{
		m_currentSong = song;

		Audio::OggVorbisDecoder* decoder = static_cast<Audio::OggVorbisDecoder*>(song->m_handle);
		if (decoder != nullptr)
			decoder->Rewind();
		else
		{
			Content::FileStream* file = new Content::FileStream(fopen(song->m_path.c_str(), "rb"));
			decoder = new Audio::OggVorbisDecoder(file, false);

			song->m_handle = decoder;
		}

		if (m_source == 0)
		{
			m_source = Audio::AudioManager::GetFreeSource(&m_source);

			alSourcei(m_source, AL_SOURCE_RELATIVE, AL_TRUE);
			alSourcei(m_source, AL_LOOPING, AL_FALSE);
			alSource3f(m_source, AL_POSITION, 0, 0, 0);
			alSource3f(m_source, AL_VELOCITY, 0, 0, 0);
			alSourcef(m_source, AL_GAIN, 1.0f);

			alGenBuffers(3, m_buffers);

			stream(m_buffers[0]);
			stream(m_buffers[1]);
			stream(m_buffers[2]);

			alSourceQueueBuffers(m_source, 3, m_buffers);
		}

		alSourceStop(m_source);
		alSourcePlay(m_source);
	}

	void MediaPlayer::Tick()
	{
		if (m_currentSong != nullptr)
		{
			int processed;
			alGetSourcei(m_source, AL_BUFFERS_PROCESSED, &processed);

			int state;
			alGetSourcei(m_source, AL_SOURCE_STATE, &state);
			
			// TODO: it's possible for the buffers to not get updated quick enough, which will cause
			// the source to stop playing. Make it pick up where it left off instead!
			//if (state != AL_PLAYING)
			//	throw Exception("zomg");

			while(processed--)
			{
				ALuint albuffer;
				alSourceUnqueueBuffers(m_source, 1, &albuffer);

				stream(albuffer);

				alSourceQueueBuffers(m_source, 1, &albuffer);
			}
		}
	}

	void MediaPlayer::stream(unsigned int alBuffer)
	{
		const int BUFFER_SIZE = 176400; // 1 second
		static byte buffer[BUFFER_SIZE];

		Audio::OggVorbisDecoder* decoder = static_cast<Audio::OggVorbisDecoder*>(m_currentSong->m_handle);
		int read = decoder->Read(buffer, BUFFER_SIZE);
		alBufferData(alBuffer, decoder->NumChannels() == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16, buffer, read, decoder->SampleRate());

		if (read == 0 && m_repeat)
			decoder->Rewind();
	}
}
}