#ifndef NXNA_MEDIA_MEDIAPLAYER_H
#define NXNA_MEDIA_MEDIAPLAYER_H

namespace Nxna
{
namespace Media
{
	class Song;

	class MediaPlayer
	{
		static Song* m_currentSong;

	public:
		static void Play(Song* song);
		static void Pause();
		static void Resume();
		static void Stop();

		static bool IsRepeating();
		static void IsRepeating(bool repeat);

		static void SetVolume(float volume);
		static float GetVolume();

		// don't use this! This is just for the AudioManager to call!
		static void Tick();

	
	};
}
}

#endif // NXNA_MEDIA_MEDIAPLAYER_H
