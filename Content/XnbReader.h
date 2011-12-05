#ifndef CONTENT_XNBREADER_H
#define CONTENT_XNBREADER_H

#include <string>

namespace Nxna
{
namespace Content
{
	class FileStream;

	enum TargetPlatform
	{
		TargetPlatform_Windows,
		TargetPlatform_WinPhone,
		TargetPlatform_XBox360
	};

	class XnbReader
	{
		FileStream* m_stream;

		TargetPlatform m_target;
		bool m_isHighDef;
		bool m_isCompressed;
		int m_compressedSize;
		int m_uncompressedSize;

	public:
		XnbReader(FileStream* stream);
		~XnbReader();

		TargetPlatform GetTargetPlatform();
		bool IsHighDef();
		bool IsCompressed();

		FileStream* GetStream() { return m_stream; }
		int ReadTypeID();

	private:

		void readHeader();
		int read7BitEncodedInt();
		std::string readString();
		void skipString();
	};
}
}

#endif // CONTENT_XNBREADER_H