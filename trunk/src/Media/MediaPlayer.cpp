#include "MediaPlayer.h"
#include "Song.h"
#include "OggMediaPlayer.h"
#include "../Audio/AudioManager.h"
#include "../Content/FileStream.h"


namespace Nxna
{
namespace Media
{
	Song* MediaPlayer::m_currentSong = nullptr;

	void MediaPlayer::Play(Song* song)
	{
		m_currentSong = song;

#ifdef NXNA_PLATFORM_APPLE_IOS

#else
		if (OggMediaPlayer::Play(song) == false)
			m_currentSong = nullptr;
#endif
	}

	void MediaPlayer::Stop()
	{
#ifdef NXNA_PLATFORM_APPLE_IOS

#else
		OggMediaPlayer::Stop();
#endif
	}

	bool MediaPlayer::IsRepeating()
	{
#ifdef NXNA_PLATFORM_APPLE_IOS
		return false;
#else
		return OggMediaPlayer::IsRepeating();
#endif
	}
	
	void MediaPlayer::IsRepeating(bool repeat)
	{
#ifdef NXNA_PLATFORM_APPLE_IOS

#else
		OggMediaPlayer::IsRepeating(repeat);
#endif
	}

	void MediaPlayer::SetVolume(float volume)
	{
#ifdef NXNA_PLATFORM_APPLE_IOS

#else
		OggMediaPlayer::SetVolume(volume);
#endif
	}

	void MediaPlayer::Tick()
	{
		if (m_currentSong != nullptr)
		{
#ifdef NXNA_PLATFORM_APPLE_IOS

#else
			OggMediaPlayer::Tick(m_currentSong->m_handle);
#endif
		}
	}
}
}