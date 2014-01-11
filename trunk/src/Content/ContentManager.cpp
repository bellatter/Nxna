#include <cstdio>
#include "ContentManager.h"
#include "FileStream.h"
#include "XnbReader.h"
#include "../Graphics/Texture2D.h"
#include "../Graphics/SpriteSheet.h"
#include "../Graphics/SpriteFont.h"
#include "../Audio/SoundEffect.h"
#include "../Media/Song.h"

#ifdef NXNA_PLATFORM_ANDROID
#include "AndroidFileSystem.h"
#endif

namespace Nxna
{
namespace Content
{
	ContentManager::ContentManager()
	{
		// add all the loaders
		AddContentReader<Nxna::Graphics::SpriteSheet>();
		AddContentReader<Nxna::Graphics::Texture2DLoader>();
		AddContentReader<Nxna::Graphics::SpriteFontLoader>();
		AddContentReader<Nxna::Audio::SoundEffectLoader>();
		AddContentReader<Nxna::Media::SongLoader>();
	}

	ContentManager::ContentManager(const char* rootDirectory)
	{
		// add all the loaders
		AddContentReader<Nxna::Graphics::SpriteSheet>();
		AddContentReader<Nxna::Graphics::Texture2DLoader>();
		AddContentReader<Nxna::Graphics::SpriteFontLoader>();
		AddContentReader<Nxna::Audio::SoundEffectLoader>();
		AddContentReader<Nxna::Media::SongLoader>();

		SetRootDirectory(rootDirectory);
	}

	ContentManager::~ContentManager()
	{
		Unload();

		// delete all the loaders
		for (LoaderMap::iterator itr = m_loaders.begin();
			itr != m_loaders.end(); itr++)
		{
			IContentReader* r = (*itr).second;
			delete r;
		}
	}

	void ContentManager::Unload()
	{
		for (ResourceMap::iterator itr = m_resources.begin();
			itr != m_resources.end(); itr++)
		{
			(*itr).second.first->Destroy((*itr).second.second);
		}

		m_resources.clear();
	}

	XnbReader* ContentManager::load(const char* name)
	{
		std::string fullName = m_rootDirectory + name + ".xnb";

#if defined NXNA_PLATFORM_ANDROID
		FileStream* fs = AndroidFileSystem::Open(fullName.c_str());
#else
		FileStream* fs = new FileStream(fullName.c_str());
#endif
		if (fs == nullptr || fs->IsOpen() == false)
			throw ContentException(std::string("Unable to open file: ") + fullName);

		return new XnbReader(fs, name, fullName.c_str(), this);
	}
}
}
