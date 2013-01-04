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
	
	class IContentReader
	{
	public:
		virtual const char* GetTypeName() = 0;
		virtual void* Read(XnbReader* reader) = 0;
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

		typedef std::map<std::string, IContentReader*> LoaderMap;
		LoaderMap m_loaders;

		typedef std::map<std::string, std::pair<IContentReader*, void*> > ResourceMap;
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

		const char* GetRootDirectory() { return m_rootDirectory.c_str(); }

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

				T* resource = static_cast<T*>((*loader).second->Read(stream));
				m_resources.insert(ResourceMap::value_type(name, std::pair<IContentReader*, void*>((*loader).second, resource)));

				delete stream;

				return resource;
			}

			return nullptr;
		}

		void Unload();

		// Regular XNA provides the ability to create custom ContentReaders. NXNA needs that too.
		// The API to do this needs to be cleaned up a little, though. But XNA can use reflection
		// to make the magic happen. We can't. So if you want to add a custom content reader you
		// have to use the following method (T = the custom content reader that implements the IContentReader interface).
		// A good place to do this is probably in your game class's Initialize() method.
		template<typename T>
		void AddContentReader()
		{
			T* t = new T();
			m_loaders.insert(LoaderMap::value_type(t->GetTypeName(), t));
		}

	private:

		XnbReader* load(const char* name);
	};
}
}

#endif // CONTENT_CONTENTMANAGER_H