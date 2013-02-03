#include "MediaPlayer.h"
#include "Song.h"
#include "../Audio/AudioManager.h"
#include "../Content/FileStream.h"

#ifdef NXNA_PLATFORM_APPLE_IOS
#include "IOSMediaPlayer.h"
#else
#include "OggMediaPlayer.h"
#endif

namespace Nxna
{
namespace Media
{
	Song* MediaPlayer::m_currentSong = nullptr;

	void MediaPlayer::Play(Song* song)
	{
		m_currentSong = song;

#ifdef NXNA_PLATFORM_APPLE_IOS
		if (IOSMediaPlayer::Play(song) == false)
			m_currentSong = nullptr;
#else
		if (OggMediaPlayer::Play(song) == false)
			m_currentSong = nullptr;
#endif
	}

	void MediaPlayer::Stop()
	{
#ifdef NXNA_PLATFORM_APPLE_IOS
		IOSMediaPlayer::Stop();
#else
		OggMediaPlayer::Stop();
#endif
	}

	bool MediaPlayer::IsRepeating()
	{
#ifdef NXNA_PLATFORM_APPLE_IOS
		return IOSMediaPlayer::IsRepeating();
#else
		return OggMediaPlayer::IsRepeating();
#endif
	}
	
	void MediaPlayer::IsRepeating(bool repeat)
	{
#ifdef NXNA_PLATFORM_APPLE_IOS
		IOSMediaPlayer::IsRepeating(repeat);
#else
		OggMediaPlayer::IsRepeating(repeat);
#endif
	}

	void MediaPlayer::SetVolume(float volume)
	{
#ifdef NXNA_PLATFORM_APPLE_IOS
		IOSMediaPlayer::SetVolume(volume);
#else
		OggMediaPlayer::SetVolume(volume);
#endif
	}

	void MediaPlayer::Tick()
	{
		if (m_currentSong != nullptr)
		{
#ifdef NXNA_PLATFORM_APPLE_IOS
			// nothing
#else
			OggMediaPlayer::Tick(m_currentSong->m_handle);
#endif
		}
	}
}
}