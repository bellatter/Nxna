#include <cstdio>
#include <cassert>
#include <cstring>
#include "FileStream.h"
#include "../MathHelper.h"

namespace Nxna
{
namespace Content
{
	FileStream::FileStream(const char* path)
	{
#if defined NXNA_PLATFORM_WIN32
		fopen_s((FILE**)&m_fp, path, "rb");
#else
		m_fp = fopen(path, "rb");
#endif
		m_bytesRead = 0;
	}

	FileStream::FileStream()
	{
		m_fp = nullptr;
		m_bytesRead = 0;
	}

	FileStream::~FileStream()
	{
		if (m_fp != nullptr)
			fclose((FILE*)m_fp);
	}

	bool FileStream::IsOpen()
	{
		return m_fp != nullptr;
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

	void FileStream::swapLE(void* /* data */, int /* length */)
	{
		// nothing... for now...
	}


	MemoryStream::MemoryStream(const byte* memory, int length)
	{
		m_weOwnBuffer = false;
		m_memory = const_cast<byte*>(memory);
		m_length = length;
		m_totalSize = length;
		m_position = 0;
	}

	MemoryStream::MemoryStream(int size)
	{
		m_weOwnBuffer = true;
		m_memory = new byte[size];
		m_length = 0;
		m_totalSize = size;
		m_position = 0;
	}

	MemoryStream::~MemoryStream()
	{
		if (m_weOwnBuffer)
			delete[] m_memory;
	}

	int MemoryStream::Read(byte* destination, int length)
	{
		int bytesToRead = Math::Min(length, m_length - m_position);

		if (bytesToRead > 0)
		{
			memcpy(destination, &m_memory[m_position], bytesToRead);

			m_position += bytesToRead;
		}

		return bytesToRead;
	}

	byte MemoryStream::ReadByte()
	{
		byte r;
		Read(&r, 1);

		return r;
	}

	short MemoryStream::ReadInt16()
	{
		short r;
		Read((byte*)&r, sizeof(short));

		swapLE(&r, sizeof(short));

		return r;
	}

	int MemoryStream::ReadInt32()
	{
		int r;
		Read((byte*)&r, sizeof(int));

		swapLE(&r, sizeof(int));

		return r;
	}

	float MemoryStream::ReadFloat()
	{
		float r;
		Read((byte*)&r, sizeof(float));

		swapLE(&r, sizeof(float));

		return r;
	}

	void MemoryStream::Seek(int offset, SeekOrigin origin)
	{
		if (origin == SeekOrigin::Begin)
			m_position = MathHelper::Clampi(offset, 0, m_length);
		else if (origin == SeekOrigin::End)
			m_position = MathHelper::Clampi(m_length + offset, 0, m_length);
		else
			m_position = MathHelper::Clampi(m_position + offset, 0, m_length);
	}

	int MemoryStream::Position()
	{
		return m_position;
	}

	int MemoryStream::Length()
	{
		return m_length;
	}
	
	bool MemoryStream::Eof()
	{
		return m_position >= m_length;
	}

	size_t MemoryStream::Write(const byte* buffer, size_t size)
	{
		// do we need to resize the buffer?
		if (size + m_position >= m_totalSize)
		{
			if (m_weOwnBuffer)
				increaseSize(Math::Max((unsigned int)(m_totalSize + size), (unsigned int)(m_totalSize * 2)));
			else
				return 0; // we don't own the buffer, so we can't expand it
		}

		memcpy(&m_memory[m_position], buffer, size);
		m_position += size;

		if (m_position > m_length)
			m_length = m_position;

		return size;
	}

	void MemoryStream::increaseSize(size_t amount)
	{
		m_totalSize += amount;

		byte* tmp = new byte[m_totalSize];
		memset(tmp, 0, m_totalSize);

		memcpy(tmp, m_memory, m_length);

		delete[] m_memory;
		m_memory = tmp;
	}

	void MemoryStream::swapLE(void* /* data */, int /* length */)
	{
		// nothing... for now...
	}

	
}
}
