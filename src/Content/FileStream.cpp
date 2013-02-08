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

	int FileStream::Read(byte* destination, int length)
	{
		int read = fread(destination, 1, length, (FILE*)m_fp);
		m_bytesRead += read;

		return read;
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

	void FileStream::Seek(int offset, SeekOrigin origin)
	{
		if (origin == SeekOrigin::Current)
		{
			fseek((FILE*)m_fp, offset, SEEK_CUR);
			m_bytesRead += offset;
		}
		else if (origin == SeekOrigin::Begin)
		{
			fseek((FILE*)m_fp, offset, SEEK_SET);
			m_bytesRead = offset;
		}
		else if (origin == SeekOrigin::End)
		{
			fseek((FILE*)m_fp, offset, SEEK_END);
			m_bytesRead = Length() - offset;
		}
	}

	int FileStream::Position()
	{
		return (int)ftell((FILE*)m_fp);
	}

	int FileStream::Length()
	{
		long current = ftell((FILE*)m_fp);
		fseek((FILE*)m_fp, 0, SEEK_END);
		int size = (int)ftell((FILE*)m_fp);
		fseek((FILE*)m_fp, current, SEEK_SET);

		return size;
	}

	bool FileStream::Eof()
	{
		return m_bytesRead >= Length(); 
	}

	void FileStream::swapLE(void* data, int length)
	{
		// nothing... for now...
	}
}
}