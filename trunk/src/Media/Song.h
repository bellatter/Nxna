#ifndef NXNA_MEDIA_SONG_H
#define NXNA_MEDIA_SONG_H

#include "../Content/ContentManager.h"

NXNA_DISABLE_OVERRIDE_WARNING

namespace Nxna
{
namespace Media
{
	class SongLoader;
	class MediaPlayer;
	class OggMediaPlayer;
	class IOSMediaPlayer;

	class Song
	{
		friend class SongLoader;
		friend class MediaPlayer;
		friend class OggMediaPlayer;
		friend class IOSMediaPlayer;

		std::string m_name;
		std::string m_path;
		void* m_handle;

		Song(const char* name, const char* path);
		~Song();

	public:
		const char* GetName() { return m_name.c_str(); }
	};

	class SongLoader : public Content::IContentReader
	{
	public:
		virtual const char* GetTypeName() override { return typeid(Song).name(); }
		virtual void* Read(Content::XnbReader* stream) override;
		virtual void Destroy(void* resource) override;
	};
}
}

NXNA_ENABLE_OVERRIDE_WARNING

#endif // NXNA_MEDIA_SONG_H
