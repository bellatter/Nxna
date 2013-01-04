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
		static int m_source;
		static unsigned int m_buffers[3];
		static bool m_repeat;

	public:
		static void Play(Song* song);
		static void Pause();
		static void Resume();
		static void Stop();

		static bool IsRepeating() { return m_repeat; }
		static void IsRepeating(bool repeat) { m_repeat = repeat; }

		// don't use this! This is just for the AudioManager to call!
		static void Tick();

	private:
		static void stream(unsigned int alBuffer);
	};
}
}

#endif // NXNA_MEDIA_MEDIAPLAYER_H
