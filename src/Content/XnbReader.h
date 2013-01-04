#ifndef CONTENT_XNBREADER_H
#define CONTENT_XNBREADER_H

#include <string>

namespace Nxna
{
namespace Content
{
	class FileStream;
	class ContentManager;

	enum TargetPlatform
	{
		TargetPlatform_Windows,
		TargetPlatform_WinPhone,
		TargetPlatform_XBox360
	};

	class XnbReader
	{
		FileStream* m_stream;
		std::string m_name;
		ContentManager* m_content;

		TargetPlatform m_target;
		bool m_isHighDef;
		bool m_isCompressed;
		int m_compressedSize;
		int m_uncompressedSize;

	public:
		XnbReader(FileStream* stream, const char* name, ContentManager* contentManager);
		~XnbReader();

		TargetPlatform GetTargetPlatform();
		bool IsHighDef();
		bool IsCompressed();

		FileStream* GetStream() { return m_stream; }
		ContentManager* GetContentManager() { return m_content; }
		const char* GetName() { return m_name.c_str(); }
		int ReadTypeID();

		std::string ReadString();

	private:
		void readHeader();
		int read7BitEncodedInt();
		
		void skipString();
	};
}
}

#endif // CONTENT_XNBREADER_H