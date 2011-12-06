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
			bool Reserved;
		};

		static SourceInfo m_sources[MAX_SOURCES];

	public:
		static void Init();
		static void Shutdown();

		static AudioSource GetFreeSource(bool reserve);
		static void ReleaseSource(AudioSource source);
	};
}
}

#endif // NXNA_AUDIO_AUDIOMANAGER_H