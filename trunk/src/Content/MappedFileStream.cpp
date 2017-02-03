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

		Load(path, this);
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

	bool MappedFileStream::Load(const char* path, MappedFileStream* result)
	{
		if (path == nullptr || result == nullptr)
			return false;

		if (result->IsOpen())
			return false;

#ifdef _WIN32
		result->m_fp = nullptr;
#else
		result->m_fd = -1;
#endif
		result->m_memory = nullptr;
		result->m_mapping = nullptr;
		result->m_length = 0;

#if defined NXNA_PLATFORM_WIN32
		result->m_fp = CreateFile(path, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, nullptr);
		if (result->m_fp == INVALID_HANDLE_VALUE)
		{
			result->m_fp = nullptr;
			return false;
		}

		result->m_mapping = CreateFileMapping(result->m_fp, nullptr, PAGE_READONLY, 0, 0, nullptr);
		if (result->m_mapping == nullptr)
		{
			CloseHandle(result->m_fp);
			result->m_fp = nullptr;
			return false;
		}

		result->m_memory = (byte*)MapViewOfFile(result->m_mapping, FILE_MAP_READ, 0, 0, 0);
		if (result->m_memory == nullptr)
		{
			CloseHandle(result->m_mapping);
			CloseHandle(result->m_fp);
			result->m_mapping = nullptr;
			result->m_fp = nullptr;
			return false;
		}

		result->m_length = GetFileSize(result->m_fp, nullptr);
#else
		result->m_fd = open(path, O_RDONLY);
		if (result->m_fd == -1)
			return false;
			
		// get the file size
		struct stat statInfo;
		if (fstat(result->m_fd, &statInfo) < 0)
		{
			close(result->m_fd);
			result->m_fd = -1;
			return false;
		}
			
		result->m_memory = (byte*)mmap(nullptr, statInfo.st_size, PROT_READ, MAP_PRIVATE, result->m_fd, 0);
		if (result->m_memory == MAP_FAILED)
		{
			close(result->m_fd);
			result->m_fd = -1;
			result->m_memory = nullptr;
			return false;
		}
			
		result->m_length = statInfo.st_size;
#endif

		result->m_position = 0;

		return true;
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
