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
	public:
		AdpcmDecoder();

		void Decode(Content::Stream* data, bool stereo, int bitrate, int blockSize, int samplesPerBlock);

		void GetOutput(const byte** output, int* outputSize);

	private:

		static std::unique_ptr<Content::MemoryStream> m_workingData;

		void copyToWorkingMemory(byte* data, unsigned int size);
	};
}
}

#endif // NXNA_AUDIO_ADPCMDECODER_H