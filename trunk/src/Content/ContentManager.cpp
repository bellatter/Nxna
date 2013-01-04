#include <cstdio>
#include "ContentManager.h"
#include "FileStream.h"
#include "XnbReader.h"
#include "../Graphics/Texture2D.h"
#include "../Graphics/SpriteSheet.h"
#include "../Graphics/SpriteFont.h"
#include "../Audio/SoundEffect.h"
#include "../Media/Song.h"

namespace Nxna
{
namespace Content
{
	ContentManager::ContentManager()
	{
		// add all the loaders
		addLoader<Nxna::Graphics::SpriteSheet>();
		addLoader<Nxna::Graphics::Texture2DLoader>();
		addLoader<Nxna::Graphics::SpriteFontLoader>();
		addLoader<Nxna::Audio::SoundEffectLoader>();
		addLoader<Nxna::Media::SongLoader>();
	}

	ContentManager::ContentManager(const char* rootDirectory)
	{
		// add all the loaders
		addLoader<Nxna::Graphics::SpriteSheet>();
		addLoader<Nxna::Graphics::Texture2DLoader>();
		addLoader<Nxna::Graphics::SpriteFontLoader>();
		addLoader<Nxna::Audio::SoundEffectLoader>();
		addLoader<Nxna::Media::SongLoader>();

		SetRootDirectory(rootDirectory);
	}

	ContentManager::~ContentManager()
	{
		Unload();

		// delete all the loaders
		for (LoaderMap::iterator itr = m_loaders.begin();
			itr != m_loaders.end(); itr++)
		{
			IContentResourceLoader* r = (*itr).second;
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

		FILE* fp = fopen(fullName.c_str(), "rb");
		if (fp == nullptr)
			throw ContentException(std::string("Unable to open file: ") + fullName);

		return new XnbReader(new FileStream(fp), name, this);
	}
}
}
