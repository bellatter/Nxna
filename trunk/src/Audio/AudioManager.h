#ifndef NXNA_AUDIO_AUDIOMANAGER_H
#define NXNA_AUDIO_AUDIOMANAGER_H

#include "../NxnaConfig.h"

namespace Nxna
{
namespace Audio
{
	typedef unsigned int AudioSource;

	class AudioManager
	{
		static void* m_device;
		static void* m_context;
		
		static const int MAX_SOURCES = 16;

		struct SourceInfo
		{
			AudioSource Source;
			void* Owner;
		};

		static SourceInfo m_sources[MAX_SOURCES];

	public:
		static void Init();
		static void Shutdown();

		static AudioSource GetFreeSource(void* owner);
		static void ReleaseSource(AudioSource source);
		static bool IsSourceOwner(AudioSource source, void* owner);

		static void SetDistanceScale(float scale);
		static void SetMasterVolume(float volume);
	};
}
}

#endif // NXNA_AUDIO_AUDIOMANAGER_H