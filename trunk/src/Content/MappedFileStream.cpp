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
		: MemoryStream(nullptr, 0)
	{
#ifdef _WIN32
		m_fp = nullptr;
#else
		m_fd = -1;
#endif
		m_mapping = nullptr;
	}

	MappedFileStream::MappedFileStream(const char* path)
		: MemoryStream(nullptr, 0)
	{
#ifdef _WIN32
		m_fp = nullptr;
#else
		m_fd = -1;
#endif

		Load(path);
	}

	MappedFileStream::~MappedFileStream()
	{
		if (IsOpen())
		{
#ifdef _WIN32
			UnmapViewOfFile(m_memory);
			CloseHandle(m_mapping);
			CloseHandle(m_fp);
#else
			munmap(m_memory, m_length);
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
			m_memory = nullptr;
			m_mapping = nullptr;
			m_length = 0;

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

			m_memory = (byte*)MapViewOfFile(m_mapping, FILE_MAP_READ, 0, 0, 0);
			if (m_memory == nullptr)
			{
				CloseHandle(m_mapping);
				CloseHandle(m_fp);
				m_mapping = nullptr;
				m_fp = nullptr;
				return;
			}

			m_length = GetFileSize(m_fp, nullptr);
#else
			m_fd = open(path, O_RDONLY);
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
			
			m_memory = (byte*)mmap(nullptr, statInfo.st_size, PROT_READ, MAP_PRIVATE, m_fd, 0);
			if (m_memory == MAP_FAILED)
			{
				close(m_fd);
				m_fd = -1;
				m_memory = nullptr;
				return;
			}
			
			m_length = statInfo.st_size;
#endif
			m_position = 0;
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
}
}