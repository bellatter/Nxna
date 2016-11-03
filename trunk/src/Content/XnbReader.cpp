#include "XnbReader.h"
#include "FileStream.h"
#include "MappedFileStream.h"
#include "ContentManager.h"
#include "../MathHelper.h"

namespace Nxna
{
namespace Content
{
	XnbReader::XnbReader(MemoryStream* stream, const char* name, const char* fullPath, ContentManager* contentManager)
	{
		m_stream = stream;
		m_name = name;
		m_fullPath = fullPath;
		m_content = contentManager;

		readHeader();
	}

	XnbReader::~XnbReader()
	{
		delete m_stream;
	}

	int XnbReader::ReadTypeID()
	{
		return read7BitEncodedInt();
	}
	
	std::string XnbReader::ReadString()
	{
		int len = read7BitEncodedInt();

		byte buffer[256];
		int bytesToRead = Math::Min(255, len);
		m_stream->Read(buffer, bytesToRead);
		buffer[bytesToRead] = 0;

		return std::string((char*)buffer);
	}

	const byte* XnbReader::GetData()
	{
		return nullptr;
	}

	void XnbReader::readHeader()
	{
#pragma pack(1)
		struct
		{
			char Magic[3];
			byte Target;
			byte Version;
			byte Flags;
			int CompressedSize;
		} header;
#pragma pack()

		static_assert(sizeof(header) == 10, "XNB header struct is unexpected size");

		if (m_stream->Read((byte*)&header, sizeof(header)) != sizeof(header))
			throw ContentException("Not a valid XNB file");

		if (header.Magic[0] != 'X' ||
			header.Magic[1] != 'N' ||
			header.Magic[2] != 'B')
			throw ContentException("Not a valid XNB file");

		if (header.Target == 'w')
			m_target = TargetPlatform::Windows;
		else if (header.Target == 'm')
			m_target = TargetPlatform::WinPhone;
		else if (header.Target == 'x')
			m_target = TargetPlatform::XBox360;
		else
			throw ContentException("Not a valid XNB file");

		if (header.Version != 5)
			throw ContentException("Not a valid XNB file");

		m_isHighDef = (header.Flags & 0x01) != 0;
		m_isCompressed = (header.Flags & 0x80) != 0;

		m_compressedSize = header.CompressedSize;
		if (m_isCompressed)
			m_uncompressedSize = m_stream->ReadInt32();
		else
			m_uncompressedSize = m_compressedSize;

		// read the type readers
		int typeReaderCount = read7BitEncodedInt();
		for (int i = 0; i < typeReaderCount; i++)
		{
			skipString();
			m_stream->ReadInt32();
		}

		// read the number of shared resources
		int numSharedResources = read7BitEncodedInt();

		// at this point the object within the file is next...
	}

	int XnbReader::read7BitEncodedInt()
	{
		int result = 0;
		int bitsRead = 0;
		int value;

		do
		{
			value = m_stream->ReadByte();
			result |= (value & 0x7f) << bitsRead;
			bitsRead += 7;
		}
		while (value & 0x80);

		return result;
	}

	void XnbReader::skipString()
	{
		int len = read7BitEncodedInt();
		
		m_stream->Seek(len, SeekOrigin::Current);
	}
}
}