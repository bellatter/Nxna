#include "Song.h"
#include "../Content/ContentManager.h"
#ifdef NXNA_PLATFORM_APPLE_IOS

#else
#include "../Audio/OggVorbis/OggVorbisDecoder.h"
#endif

namespace Nxna
{
namespace Media
{
	Song::Song(const char* name)
	{
		m_name = new char[strlen(name) + 1];
		strcpy(m_name, name);

		m_path = nullptr;
		m_handle = nullptr;
	}

	Song::~Song()
	{
		delete[] m_name;

		if (m_path)
			delete[] m_path;

#ifdef NXNA_PLATFORM_APPLE_IOS
#else
		if (m_handle != nullptr)
			delete static_cast<Audio::OggVorbisDecoder*>(m_handle);
#endif
	}

	void* SongLoader::Read(Content::XnbReader* stream)
	{
		stream->GetContentManager();
		stream->ReadTypeID();
		std::string path = stream->ReadString();

		std::string pathWithoutExtension = path.substr(0, path.find_last_of('.'));

		char audioFile[256];
#ifdef NXNA_PLATFORM_APPLE_IOS
		snprintf(audioFile, 256, "%s/%s.mp3", stream->GetContentManager()->GetRootDirectory(), stream->GetName());
#elif defined NXNA_PLATFORM_WIN32
		_snprintf_s(audioFile, 256, "%s/%s.ogg", stream->GetContentManager()->GetRootDirectory(), stream->GetName());
#else
		snprintf(audioFile, 256, "%s/%s.ogg", stream->GetContentManager()->GetRootDirectory(), stream->GetName());
#endif

		Song* song = new Song(pathWithoutExtension.c_str());
		song->m_path = new char[strlen(audioFile) + 1];
		strcpy(song->m_path, audioFile);

		return song;
	}

	void* SongLoader::ReadRaw(Content::MemoryStream* stream, bool* keepStreamOpen)
	{
		*keepStreamOpen = true;

		return Create("unknown", stream);
	}

	Song* SongLoader::Create(const char* name, Content::Stream* stream)
	{
		Song* result = new Song(name);

#ifdef NXNA_PLATFORM_APPLE_IOS
		// TODO
#else
		result->m_handle = new Audio::OggVorbisDecoder(stream, true);
#endif

		return result;
	}

	void SongLoader::Destroy(void* resource)
	{
		delete static_cast<Song*>(resource);
	}
}
}