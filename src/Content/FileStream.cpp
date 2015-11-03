#include <cstdio>
#include <cassert>
#include <cstring>
#include "FileStream.h"
#include "../MathHelper.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#ifdef __APPLE__
#include <sys/uio.h>
#else
#include <io.h>
#endif
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#ifndef O_BINARY
#define O_BINARY 0
#endif
#endif


namespace Nxna
{
namespace Content
{
	FileStream::FileStream(const char* path)
	{
#if defined NXNA_PLATFORM_WIN32
		m_fp = CreateFile(path, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
#else
		m_fp = open(path, O_BINARY | O_RDONLY);
#endif
		m_bytePosition = 0;
		m_bufferPosition = 0;
		m_bufferSize = 0;
	}

	FileStream::FileStream()
	{
#ifdef _WIN32
		m_fp = nullptr;
#else
		m_fp = -1;
#endif
		m_bytePosition = 0;
		m_bufferPosition = 0;
		m_bufferSize = 0;
	}

	FileStream::~FileStream()
	{
#ifdef _WIN32
		if (m_fp != nullptr)
			CloseHandle(m_fp);
#else
		if (m_fp != -1)
			close(m_fp);
#endif
	}

	bool FileStream::IsOpen()
	{
#ifdef _WIN32
		return m_fp != nullptr;
#else
		return m_fp != -1;
#endif
	}

	int FileStream::Read(byte* destination, unsigned int length)
	{
		auto bufferBytesToCopy = m_bufferSize - m_bufferPosition;
		unsigned int fileBytesToCopy;
		if (length < bufferBytesToCopy)
		{
			bufferBytesToCopy = length;
			fileBytesToCopy = 0;
		}
		else
		{
			fileBytesToCopy = length - bufferBytesToCopy;
		}

		memcpy(destination, m_buffer + m_bufferPosition, bufferBytesToCopy);
		m_bufferPosition += bufferBytesToCopy;

		if (fileBytesToCopy > 0)
		{
			if (fileBytesToCopy > m_maxBufferSize)
			{
#ifdef _WIN32
				DWORD r = 0;
				ReadFile(m_fp, destination + bufferBytesToCopy, fileBytesToCopy, &r, nullptr);
#else
				auto r = read((int)m_fp, destination + bufferBytesToCopy, fileBytesToCopy);
#endif
				m_bytePosition += r;
				m_bufferSize = 0;
				m_bufferPosition = 0;

				return bufferBytesToCopy + r;
			}

#ifdef _WIN32
			DWORD numBytesRead = 0;
			ReadFile(m_fp, m_buffer, m_maxBufferSize, &numBytesRead, nullptr);
			m_bufferSize = numBytesRead;
#else
			m_bufferSize = read(m_fp, m_buffer, m_maxBufferSize);
#endif
			if (m_bufferSize < fileBytesToCopy)
				fileBytesToCopy = m_bufferSize;

			memcpy(destination + bufferBytesToCopy, m_buffer, fileBytesToCopy);
			m_bufferPosition = fileBytesToCopy;
		}

		m_bytePosition += bufferBytesToCopy + fileBytesToCopy;
		return bufferBytesToCopy + fileBytesToCopy;
	}

	byte FileStream::ReadByte()
	{
		// ReadByte() is a special implementation to help speed things up that need
		// to read files 1 byte at a time

		if (m_bufferPosition == m_bufferSize)
		{
#ifdef _WIN32
			DWORD numBytesRead = 0;
			ReadFile(m_fp, m_buffer, m_maxBufferSize, &numBytesRead, nullptr);
			m_bufferSize = numBytesRead;
#else
			m_bufferSize = read((int)m_fp, m_buffer, m_maxBufferSize);
#endif
			m_bufferPosition = 0;
		}

		byte r = m_buffer[m_bufferPosition];
		m_bufferPosition++;
		m_bytePosition++;

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
			m_bytePosition += offset;
		}
		else if (origin == SeekOrigin::Begin)
		{
			m_bytePosition = offset;
		}
		else if (origin == SeekOrigin::End)
		{
			m_bytePosition = Length() - offset;
		}

#ifdef _WIN32
		SetFilePointer(m_fp, m_bytePosition, nullptr, FILE_BEGIN);
#else
		lseek(m_fp, m_bytePosition, SEEK_SET);
#endif
		m_bufferPosition = 0;
		m_bufferSize = 0;
	}

	int FileStream::Position()
	{
		return m_bytePosition;
	}

	int FileStream::Length()
	{
#ifdef _WIN32
		return (int)GetFileSize(m_fp, nullptr);
#else
		struct stat s;
		fstat((int)m_fp, &s);
		return s.st_size;
#endif

		
	}

	bool FileStream::Eof()
	{
		return m_bytePosition >= Length(); 
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

	int MemoryStream::Read(byte* destination, unsigned int length)
	{
		int bytesToRead = length < m_length - m_position ? length : m_length - m_position;

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
