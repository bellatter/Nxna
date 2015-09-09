#include "MappedFileStream.h"
#include "../MathHelper.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#endif
#include <cstring>

namespace Nxna
{
namespace Content
{
	MappedFileStream::MappedFileStream()
	{
#ifdef _WIN32
		m_fp = nullptr;
#else
		m_fd = -1;
#endif
		m_mapping = nullptr;
		m_baseAddress = nullptr;
		m_size = 0;
		m_bytesRead = 0;
	}

	MappedFileStream::MappedFileStream(const char* path)
	{
#ifdef _WIN32
		m_fp = nullptr;
#else
		m_fd = -1;
#endif
		m_size = 0;

		Load(path);
	}

	MappedFileStream::~MappedFileStream()
	{
		if (IsOpen())
		{
#ifdef _WIN32
			UnmapViewOfFile(m_baseAddress);
			CloseHandle(m_mapping);
			CloseHandle(m_fp);
#else
			munmap(m_mapping, m_size);
			close(m_fd);
#endif
		}
	}

	void MappedFileStream::Load(const char* path)
	{
		if (IsOpen() == false)
		{
#ifdef _WIN32
			m_fp = nullptr;
#else
			m_fd = -1;
#endif
			m_mapping = nullptr;
			m_baseAddress = nullptr;
			m_size = 0;

#if defined NXNA_PLATFORM_WIN32
			m_fp = CreateFile(path, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, nullptr);
			if (m_fp == INVALID_HANDLE_VALUE)
			{
				m_fp = nullptr;
				return;
			}

			m_mapping = CreateFileMapping(m_fp, nullptr, PAGE_READONLY, 0, 0, nullptr);
			if (m_mapping == nullptr)
			{
				CloseHandle(m_fp);
				m_fp = nullptr;
				return;
			}

			m_baseAddress = MapViewOfFile(m_mapping, FILE_MAP_READ, 0, 0, 0);
			if (m_baseAddress == nullptr)
			{
				CloseHandle(m_mapping);
				CloseHandle(m_fp);
				m_mapping = nullptr;
				m_fp = nullptr;
				return;
			}

			m_size = GetFileSize(m_fp, nullptr);
#else
			m_fd = open(path, O_RDONLY, 0);
			if (m_fd == -1)
				return;
			
			// get the file size
			struct stat statInfo;
			if (fstat(m_fd, &statInfo) < 0)
			{
				close(m_fd);
				m_fd = -1;
				return;
			}
			
			m_baseAddress = mmap(nullptr, statInfo.st_size, PROT_READ, MAP_PRIVATE, m_fd, 0);
			if (m_baseAddress == MAP_FAILED)
			{
				close(m_fd);
				m_fd = -1;
				m_baseAddress = nullptr;
				return;
			}
			
			m_size = statInfo.st_size;
#endif
			m_bytesRead = 0;
		}
	}

	bool MappedFileStream::IsOpen()
	{
#ifdef _WIN32
		return m_fp != nullptr;
#else
		return m_fd != -1;
#endif
	}

	int MappedFileStream::Read(byte* destination, int length)
	{
		if (m_bytesRead + length > m_size)
			length = m_size - m_bytesRead;

		if (length > 0)
			memcpy(destination, (char*)m_baseAddress + m_bytesRead, length);

		m_bytesRead += length;

		return length;
	}

	byte MappedFileStream::ReadByte()
	{
		byte r;
		Read(&r, 1);

		return r;
	}

	short MappedFileStream::ReadInt16()
	{
		short r;
		Read((byte*)&r, sizeof(short));

		swapLE(&r, sizeof(short));

		return r;
	}

	int MappedFileStream::ReadInt32()
	{
		int r;
		Read((byte*)&r, sizeof(int));

		swapLE(&r, sizeof(int));

		return r;
	}

	float MappedFileStream::ReadFloat()
	{
		float r;
		Read((byte*)&r, sizeof(float));

		swapLE(&r, sizeof(float));

		return r;
	}

	void MappedFileStream::Seek(int offset, SeekOrigin origin)
	{
		if (origin == SeekOrigin::Begin)
		{
			m_bytesRead = Nxna::MathHelper::Clampi(offset, 0, m_size);
		}
		else if (origin == SeekOrigin::Current)
		{
			m_bytesRead = Nxna::MathHelper::Clampi(m_bytesRead + offset, 0, m_size);
		}
		else if (origin == SeekOrigin::End)
		{
			m_bytesRead = Nxna::MathHelper::Clampi(m_size + offset, 0, m_size);
		}
	}

	void MappedFileStream::swapLE(void* /* data */, int /* length */)
	{
		// nothing... for now...
	}
}
}