#include "MappedFileStream.h"
#include "../MathHelper.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

namespace Nxna
{
namespace Content
{
	MappedFileStream::MappedFileStream()
	{
		m_fp = nullptr;
		m_mapping = nullptr;
		m_baseAddress = nullptr;
		m_size = 0;
		m_bytesRead = 0;
	}

	MappedFileStream::MappedFileStream(const char* path)
	{
		m_fp = nullptr;
		m_size = 0;

		Load(path);
	}

	MappedFileStream::~MappedFileStream()
	{
		if (IsOpen())
		{
			UnmapViewOfFile(m_baseAddress);
			CloseHandle(m_mapping);
			CloseHandle(m_fp);
		}
	}

	void MappedFileStream::Load(const char* path)
	{
		if (IsOpen() == false)
		{
			m_fp = nullptr;
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
			m_fp = fopen(path, "rb");
#endif
			m_bytesRead = 0;
		}
	}

	bool MappedFileStream::IsOpen()
	{
		return m_fp != nullptr;
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