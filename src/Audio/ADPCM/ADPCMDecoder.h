#ifndef NXNA_AUDIO_ADPCMDECODER_H
#define NXNA_AUDIO_ADPCMDECODER_H

#include "../../NxnaConfig.h"

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

		void Decode(Content::FileStream* data, bool stereo, int bitrate, int blockSize, int samplesPerBlock);

		void GetOutput(byte** output, int* outputSize)
		{
			*output = m_workingData;
			*outputSize = m_bytesWritten;
		}

	private:
	
		int m_bytesWritten;

		static byte* m_workingData;
		static int m_workingDataSize;

		void copyToWorkingMemory(byte* data, int size);
	};
}
}

#endif // NXNA_AUDIO_ADPCMDECODER_H