#ifndef NXNA_CONTENT_ANDROIDFILESYSTEM_H
#define NXNA_CONTENT_ANDROIDFILESYSTEM_H

namespace Nxna
{
namespace Content
{
	class FileStream;

	class AndroidFileSystem
	{
		static void* m_androidAssetManager;
		static void* m_openFd;
		static void* m_open;

	public:
		static void Initialize(void* assetManager);
		static void Shutdown();

		static FileStream* Open(const char* path);
	};
}
}

#endif // NXNA_CONTENT_ANDROIDFILESYSTEM
