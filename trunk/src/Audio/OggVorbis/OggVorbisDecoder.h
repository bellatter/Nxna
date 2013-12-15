#ifndef NXNA_AUDIO_OGGVORBISDECODER_H
#define NXNA_AUDIO_OGGVORBISDECODER_H

#include "../../NxnaConfig.h"

#ifndef NXNA_DISABLE_OGG

#if defined NXNA_PLATFORM_APPLE && __cplusplus < 200103L
#include <tr1/cstdint>
#else
#include <cstdint>
#endif
#include "vorbis/vorbisfile.h"
#include "../../NxnaConfig.h"

namespace Nxna
{
namespace Content
{
	class FileStream;
}

namespace Audio
{
	class OggVorbisDecoder
	{
		int m_numChannels;
		int m_sampleRate;
		int m_fileStreamStartOffset;
		Content::FileStream* m_file;
		OggVorbis_File m_vorbisFile;
		bool m_loop;

	public:

		OggVorbisDecoder(Content::FileStream* file, bool loop);
		~OggVorbisDecoder();

		int Read(byte* buffer, int bufferSize);
		void Rewind();

		int NumChannels() { return m_numChannels; }
		int SampleRate() { return m_sampleRate; }

	private:
		static size_t read_callback(void* ptr, size_t size, size_t nmemb, void* datasource);
		static int seek_callback(void* datasource, int64_t offset, int whence);
		static long tell_callback(void* datasource);
	};
}
}

#endif // NXNA_DISABLE_OGG

#endif // NXNA_AUDIO_OGGVORBISDECODER_H
