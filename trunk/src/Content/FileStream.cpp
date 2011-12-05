#include <cstdio>
#include <cassert>
#include "FileStream.h"

namespace Nxna
{
namespace Content
{
	FileStream::FileStream(void* fp)
	{
		assert(fp != nullptr);

		m_fp = fp;
		m_bytesRead = 0;
	}

	FileStream::~FileStream()
	{
		fclose((FILE*)m_fp);
	}

	void FileStream::Read(byte* destination, int length)
	{
		fread(destination, 1, length, (FILE*)m_fp);
		m_bytesRead += length;
	}

	byte FileStream::ReadByte()
	{
		byte r;
		Read(&r, 1);

		return r;
	}

	short FileStream::ReadInt16()
	{
		short r;
		Read((byte*)&r, sizeof(short));

		swapLE(&r, sizeof(short));

		return r;
	}

	int FileStream::ReadInt32()
	{
		int r;
		Read((byte*)&r, sizeof(int));

		swapLE(&r, sizeof(int));

		return r;
	}

	float FileStream::ReadFloat()
	{
		float r;
		Read((byte*)&r, sizeof(float));

		swapLE(&r, sizeof(float));

		return r;
	}

	void FileStream::Advance(int bytes)
	{
		fseek((FILE*)m_fp, bytes, SEEK_CUR);
		m_bytesRead += bytes;
	}

	void FileStream::swapLE(void* data, int length)
	{
		// nothing... for now...
	}
}
}