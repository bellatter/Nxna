#include <cstring>
#include <climits>
#include <cassert>
#include "../../Content/FileStream.h"
#include "ADPCMDecoder.h"

namespace Nxna
{
namespace Audio
{
	AdpcmDecoder::AdpcmDecoder(Content::Stream* data, bool stereo, int bitrate, int blockSize, int samplesPerBlock)
	{
		m_stream = data;
		m_stereo = stereo;
		m_bitrate = bitrate;
		m_blockSize = blockSize;
		m_samplesPerBlock = samplesPerBlock;

		size_t sizePerBlock = m_samplesPerBlock * (stereo ? 2 : 1) * 2;
		size_t numBlocks = (m_stream->Length() - m_stream->Position()) / m_blockSize;
		m_requiredOutputBufferSize = numBlocks * sizePerBlock;
	}

	struct AdpcmInfo
	{
		byte NumChannels;
		byte Nibble;
		bool NibbleState;
		short SamplesPerBlock;

		short* Coefficients1;
		short* Coefficients2;
	};

	struct AdpcmBlockHeader
	{
		byte Predictor;
		short Delta;
		short Sample1;
		short Sample2;
	};

	void decodeNibble(byte nibble, AdpcmBlockHeader* header, int predictedSample)
	{
		assert(nibble < 16);

		int AdaptationTable[] = { 
			230, 230, 230, 230, 307, 409, 512, 614, 
			768, 614, 512, 409, 307, 230, 230, 230 
		};

		int sample;
		if (nibble & 0x08)
			sample = predictedSample + header->Delta * (nibble - 0x10);
		else
			sample = predictedSample + header->Delta * nibble;

		// clamp
		if (sample > SHRT_MAX)
			sample = SHRT_MAX;
		else if (sample < SHRT_MIN)
			sample = SHRT_MIN;

		header->Delta = (short)(header->Delta * AdaptationTable[nibble] / 256);
		header->Sample2 = header->Sample1;
		header->Sample1 = (short)sample;

		if (header->Delta < 16)
			header->Delta = 16;
	}

	int decodeFrame(AdpcmInfo* info, AdpcmBlockHeader* headers, Content::Stream* data)
	{
		int bytesRead = 0;
		for (int i = 0; i < info->NumChannels; i++)
		{
			AdpcmBlockHeader* header = &headers[i];
			short coeff1 = info->Coefficients1[header->Predictor];
			short coeff2 = info->Coefficients2[header->Predictor];

			int predictedSample = ((header->Sample1 * coeff1) + (header->Sample2 * coeff2)) / 256;

			if (!info->NibbleState)
			{
				info->Nibble = data->ReadByte();
				decodeNibble(info->Nibble >> 4, header, predictedSample);

				bytesRead++;
			}
			else
			{
				decodeNibble(info->Nibble & 0x0F, header, predictedSample);
			}

			info->NibbleState = !info->NibbleState;
		}

		return bytesRead;
	}

	int decodeStereoBlock(AdpcmInfo* info, Content::Stream* block, byte* output)
	{
		// this is based on the info here:
		// http://wiki.multimedia.cx/index.php?title=Microsoft_ADPCM

		// read the headers
		AdpcmBlockHeader headers[2];
		headers[0].Predictor = block->ReadByte();
		headers[1].Predictor = block->ReadByte();
		headers[0].Delta = block->ReadInt16();
		headers[1].Delta = block->ReadInt16();
		headers[0].Sample1 = block->ReadInt16();
		headers[1].Sample1 = block->ReadInt16();
		headers[0].Sample2 = block->ReadInt16();
		headers[1].Sample2 = block->ReadInt16();

		assert(headers[0].Predictor < 7);
		assert(headers[1].Predictor < 7);

		// write the first two samples
		memcpy(output, &headers[0].Sample2, 2);
		memcpy(output + 2, &headers[1].Sample2, 2);
		memcpy(output + 4, &headers[0].Sample1, 2);
		memcpy(output + 6, &headers[1].Sample1, 2);

		// begin decoding
		int samplesRead = 2;
		int bytesWritten = 8;
		while(samplesRead < info->SamplesPerBlock)
		{
			decodeFrame(info, headers, block);

			memcpy(output + bytesWritten, &headers[0].Sample1, 2);
			memcpy(output + bytesWritten + 2, &headers[1].Sample1, 2);

			bytesWritten += 4;

			samplesRead++;
		}

		return bytesWritten;
	}

	void AdpcmDecoder::Decode(byte* outputBuffer)
	{
		short AdaptCoeff1[] = { 256, 512, 0, 192, 240, 460, 392 };
		short AdaptCoeff2[] = { 0, -256, 0, 64, 0, -208, -232 };

		AdpcmInfo info;
		info.NumChannels = (m_stereo ? 2 : 1);
		info.NibbleState = false;
		info.SamplesPerBlock = (short)m_samplesPerBlock;
		info.Coefficients1 = AdaptCoeff1;
		info.Coefficients2 = AdaptCoeff2;

		if (m_stereo)
		{
			int bytesWritten = 0;
			int blocksDecoded = 0;
			
			while(m_stream->Length() - m_stream->Position() >= m_blockSize)
			{
				assert(bytesWritten + m_samplesPerBlock * 4 <= (int)m_requiredOutputBufferSize);
				bytesWritten += decodeStereoBlock(&info, m_stream, outputBuffer + bytesWritten);

				blocksDecoded++;
			}
		}
	}
}
}