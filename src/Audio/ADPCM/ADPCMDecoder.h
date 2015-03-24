#ifndef NXNA_AUDIO_ADPCMDECODER_H
#define NXNA_AUDIO_ADPCMDECODER_H

#include "../../NxnaConfig.h"
#include <memory>

namespace Nxna
{
	namespace Content
	{
		class FileStream;
		class MemoryStream;
	}

namespace Audio
{
	class AdpcmDecoder
	{
		Content::Stream* m_stream;
		bool m_stereo;
		int m_bitrate;
		int m_blockSize;
		int m_samplesPerBlock;
		size_t m_requiredOutputBufferSize;

	public:
		AdpcmDecoder(Content::Stream* data, bool stereo, int bitrate, int blockSize, int samplesPerBlock);

		size_t GetRequiredBufferSize() { return m_requiredOutputBufferSize; }

		void Decode(byte* outputBuffer);
	};
}
}

#endif // NXNA_AUDIO_ADPCMDECODER_H