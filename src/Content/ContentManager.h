#ifndef CONTENT_CONTENTMANAGER_H
#define CONTENT_CONTENTMANAGER_H

#include <typeinfo>
#include <map>
#include <string>
#include "../NxnaConfig.h"
#include "XnbReader.h"
#include "../Exception.h"

namespace Nxna
{
namespace Content
{
	class FileStream;
	class XnbReader;
	
	class IContentResourceLoader
	{
	public:
		virtual const char* GetTypeName() = 0;
		virtual void* Load(XnbReader* reader) = 0;
		virtual void Destroy(void* resource) = 0;
	};

	class ContentException : public Exception
	{
	public:
		ContentException(const std::string& message)
			: Exception(message)
		{
		}
	};

	class ContentManager
	{
		std::string m_rootDirectory;

		typedef std::map<std::string, IContentResourceLoader*> LoaderMap;
		LoaderMap m_loaders;

		typedef std::map<std::string, std::pair<IContentResourceLoader*, void*> > ResourceMap;
		ResourceMap m_resources;

	public:

		ContentManager();
		ContentManager(const char* rootDirectory);
		~ContentManager();

		void SetRootDirectory(const char* rootDirectory)
		{
			m_rootDirectory = rootDirectory;

			if (m_rootDirectory[m_rootDirectory.length() - 1] != '/')
				m_rootDirectory += '/';
		}

		template<typename T>
		T* Load(const char* name)
		{
			// is the resource already loaded?
			ResourceMap::iterator r = m_resources.find(name);
			if (r != m_resources.end())
				return static_cast<T*>((*r).second.second);

			// the resource hasn't been loaded yet, so load it
			const char* typeName = typeid(T).name();
			LoaderMap::iterator loader = m_loaders.find(typeName);
			
			if (loader != m_loaders.end())
			{
				// load the raw resource data from a file
				XnbReader* stream = load(name);

				T* resource = static_cast<T*>((*loader).second->Load(stream));
				m_resources.insert(ResourceMap::value_type(name, std::pair<IContentResourceLoader*, void*>((*loader).second, resource)));

				delete stream;

				return resource;
			}

			return nullptr;
		}

		void Unload();

	private:

		XnbReader* load(const char* name);

		template<typename T>
		void addLoader()
		{
			T* t = new T();
			m_loaders.insert(LoaderMap::value_type(t->GetTypeName(), t));
		}
	};
}
}

#endif // CONTENT_CONTENTMANAGER_H