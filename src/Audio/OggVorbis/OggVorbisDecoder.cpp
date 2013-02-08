#include "OggVorbisDecoder.h"
#ifndef NXNA_DISABLE_OGG

#include "../../Content/FileStream.h"
#include "../../Exception.h"
#include "vorbis/vorbisfile.h"

namespace Nxna
{
namespace Audio
{
	OggVorbisDecoder::OggVorbisDecoder(Content::FileStream* file, bool loop)
	{
		m_file = file;
		m_fileStreamStartOffset = file->Position();
		m_loop = loop;

		ov_callbacks callbacks;
		callbacks.read_func = read_callback;
		callbacks.seek_func = seek_callback;
		callbacks.tell_func = tell_callback;
		callbacks.close_func = nullptr;

		if (ov_open_callbacks(this, &m_vorbisFile, nullptr, 0, callbacks) != 0)
			throw Exception("Unable to setup ogg file for reading", __FILE__, __LINE__);

		vorbis_info* info = ov_info(&m_vorbisFile, -1);
		m_numChannels = info->channels;
		m_sampleRate = info->rate;
	}

	OggVorbisDecoder::~OggVorbisDecoder()
	{
		delete m_file;
		ov_clear(&m_vorbisFile);
	}

	int OggVorbisDecoder::Read(byte* buffer, int bufferSize)
	{
		int size = 0;
		int section;
		bool stopWhenDone = false;

		while(size < bufferSize)
		{
			int r = ov_read(&m_vorbisFile, (char*)(buffer + size), bufferSize - size, 0, 2, 1, &section);

			if (r > 0) size += r;
			else if (r == 0)
			{
				if (m_loop)
					ov_time_seek(&m_vorbisFile, 0);
				else
					break;
			}
			else
			{
				break;
			}
		}

		return size;
	}

	void OggVorbisDecoder::Rewind()
	{
		ov_time_seek(&m_vorbisFile, 0);
	}

	size_t OggVorbisDecoder::read_callback(void* ptr, size_t size, size_t nmemb, void* datasource)
	{
		OggVorbisDecoder* decoder = static_cast<OggVorbisDecoder*>(datasource);
		return decoder->m_file->Read((byte*)ptr, size);
	}

	int OggVorbisDecoder::seek_callback(void* datasource, int64_t offset, int whence)
	{
		OggVorbisDecoder* decoder = static_cast<OggVorbisDecoder*>(datasource);
		if (whence == SEEK_SET) decoder->m_file->Seek((int)offset, Content::SeekOrigin::Begin);
		else if (whence == SEEK_CUR) decoder->m_file->Seek((int)offset, Content::SeekOrigin::Current);
		else if (whence == SEEK_END) decoder->m_file->Seek((int)offset, Content::SeekOrigin::End);

		// TODO: our seek function returns nothing, so there's no way for Vorbis to know about an error!
		return 0;
	}

	long OggVorbisDecoder::tell_callback(void* datasource)
	{
		OggVorbisDecoder* decoder = static_cast<OggVorbisDecoder*>(datasource);
		return decoder->m_file->Position();
	}
}
}

#endif // NXNA_DISABLE_OGG