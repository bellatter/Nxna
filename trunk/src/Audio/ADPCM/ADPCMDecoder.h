#ifndef NXNA_AUDIO_ADPCMDECODER_H
#define NXNA_AUDIO_ADPCMDECODER_H

#include "../../NxnaConfig.h"
#include <vector>

namespace Nxna
{
	namespace Content
	{
		class FileStream;
	}

namespace Audio
{
	class AdpcmDecoder
	{
	public:
		AdpcmDecoder();

		void Decode(Content::Stream* data, bool stereo, int bitrate, int blockSize, int samplesPerBlock);

		void GetOutput(byte** output, int* outputSize)
		{
			*output = &m_workingData.front();
			*outputSize = m_bytesWritten;
		}

	private:
	
		unsigned int m_bytesWritten;

		static std::vector<byte> m_workingData;

		void copyToWorkingMemory(byte* data, unsigned int size);
	};
}
}

#endif // NXNA_AUDIO_ADPCMDECODER_H