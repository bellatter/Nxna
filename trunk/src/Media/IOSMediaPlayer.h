#ifndef NXNA_MEDIA_IOSMEDIAPLAYER_H
#define NXNA_MEDIA_IOSMEDIAPLAYER_H

namespace Nxna
{
namespace Media
{
	class Song;
	
	class IOSMediaPlayer
	{
		static void* m_currentPlayer;
		
	public:
		static bool Play(Song* song);
		static void Pause();
		static void Resume();
		static void Stop();
		
		static bool IsRepeating();
		static void IsRepeating(bool repeat);
		
		static void SetVolume(float volume);
		static float GetVolume();
	};
}
}

#endif // NXNA_MEDIA_IOSMEDIAPLAYER_H
