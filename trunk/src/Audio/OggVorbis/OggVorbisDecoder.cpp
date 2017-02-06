#include "OggVorbisDecoder.h"
#ifndef NXNA_DISABLE_OGG

#include "../../Content/FileStream.h"
#include "../../Exception.h"

namespace Nxna
{
namespace Audio
{
	OggVorbisDecoder::OggVorbisDecoder(Content::MemoryStream* file, bool loop)
	{
		m_file = file;
		m_fileStreamStartOffset = file->Position();
		m_loop = loop;

		m_vorbisFile = stb_vorbis_open_memory(file->GetBuffer(), file->Length(), nullptr, nullptr);
		if (m_vorbisFile == nullptr)
			throw Exception("Unable to setup ogg file for reading", __FILE__, __LINE__);

		auto info = stb_vorbis_get_info(m_vorbisFile);

		m_numChannels = info.channels;
		m_sampleRate = info.sample_rate;
	}

	OggVorbisDecoder::~OggVorbisDecoder()
	{
		delete m_file;

		stb_vorbis_close(m_vorbisFile);
	}

	int OggVorbisDecoder::Read(byte* buffer, int bufferSize)
	{
		int size = 0;
		int section;
		bool stopWhenDone = false;

		int numShorts = bufferSize / sizeof(short);
		size = stb_vorbis_get_samples_short_interleaved(m_vorbisFile, m_numChannels, (short*)buffer, numShorts);

		return size * m_numChannels * sizeof(short);
	}

	void OggVorbisDecoder::Rewind()
	{
		stb_vorbis_seek_start(m_vorbisFile);
	}
}
}

#endif // NXNA_DISABLE_OGG