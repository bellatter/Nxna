#ifndef NXNA_AUDIO_OGGVORBISDECODER_H
#define NXNA_AUDIO_OGGVORBISDECODER_H

#include "../../NxnaConfig.h"

#ifndef NXNA_DISABLE_OGG

#if defined NXNA_PLATFORM_APPLE && __cplusplus < 200103L
#include <tr1/cstdint>
#else
#include <cstdint>
#endif

#define STB_VORBIS_HEADER_ONLY
#include "VorbisImpl.c"

#include "../../NxnaConfig.h"

namespace Nxna
{
namespace Content
{
	class MemoryStream;
}

namespace Audio
{
	class OggVorbisDecoder
	{
		int m_numChannels;
		int m_sampleRate;
		int m_fileStreamStartOffset;
		stb_vorbis* m_vorbisFile;
		Content::MemoryStream* m_file;
		bool m_loop;

	public:

		OggVorbisDecoder(Content::MemoryStream* file, bool loop);
		~OggVorbisDecoder();

		int Read(byte* buffer, int bufferSize);
		void Rewind();

		int NumChannels() { return m_numChannels; }
		int SampleRate() { return m_sampleRate; }
	};
}
}

#endif // NXNA_DISABLE_OGG

#endif // NXNA_AUDIO_OGGVORBISDECODER_H
