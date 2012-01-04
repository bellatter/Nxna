#ifndef CONTENT_FILESTREAM_H
#define CONTENT_FILESTREAM_H

#include "../NxnaConfig.h"

namespace Nxna
{
namespace Content
{
	class FileStream
	{
		void* m_fp;
		int m_bytesRead;

	public:
		FileStream(void* fp);
		~FileStream();

		void Read(byte* destination, int length);
		int ReadInt32();
		short ReadInt16();
		float ReadFloat();
		byte ReadByte();

		void Advance(int bytes);

		int Position();
		int Length();
		bool Eof();

	private:
		void swapLE(void* data, int length);
	};
}
}

#endif 