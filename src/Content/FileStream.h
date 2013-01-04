#ifndef CONTENT_FILESTREAM_H
#define CONTENT_FILESTREAM_H

#include "../NxnaConfig.h"

namespace Nxna
{
namespace Content
{
	enum SeekOrigin
	{
		SeekOrigin_Begin,
		SeekOrigin_Current,
		SeekOrigin_End
	};

	class FileStream
	{
		void* m_fp;
		int m_bytesRead;

	public:
		FileStream(void* fp);
		~FileStream();

		int Read(byte* destination, int length);
		int ReadInt32();
		short ReadInt16();
		float ReadFloat();
		byte ReadByte();

		void Seek(int offset, SeekOrigin origin);
		void Advance(int bytes) { Seek(bytes, SeekOrigin_Current); }

		int Position();
		int Length();
		bool Eof();

	private:
		void swapLE(void* data, int length);
	};
}
}

#endif 