#include "Song.h"
#include "../Content/ContentManager.h"

namespace Nxna
{
namespace Media
{
	Song::Song(const char* name, const char* path)
	{
		m_name = name;
		m_path = path;
		m_handle = nullptr;
	}

	void* SongLoader::Load(Content::XnbReader* stream)
	{
		stream->GetContentManager();
		int typeID = stream->ReadTypeID();
		std::string path = stream->ReadString();

		std::string pathWithoutExtension = path.substr(0, path.find_last_of('.'));

		char audioFile[256];
#ifdef NXNA_PLATFORM_WIN32
		_snprintf(audioFile, 256, "%s/%s.ogg", stream->GetContentManager()->GetRootDirectory(), stream->GetName());
#else
		snprintf(audioFile, 256, "%s/%s.ogg", stream->GetContentManager()->GetRootDirectory(), stream->GetName());
#endif
		Song* song = new Song(pathWithoutExtension.c_str(), audioFile);

		return song;
	}

	void SongLoader::Destroy(void* resource)
	{
		delete static_cast<Song*>(resource);
	}
}
}