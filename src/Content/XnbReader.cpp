#include "XnbReader.h"
#include "FileStream.h"
#include "ContentManager.h"
#include "../MathHelper.h"

namespace Nxna
{
namespace Content
{
	XnbReader::XnbReader(FileStream* stream)
	{
		m_stream = stream;

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
		m_stream->Read(buffer, Math::Min(256, len));
		buffer[255] = 0;

		return std::string((char*)buffer);
	}

	void XnbReader::readHeader()
	{
		if (m_stream->ReadByte() != 'X' ||
			m_stream->ReadByte() != 'N' ||
			m_stream->ReadByte() != 'B')
			throw ContentException("Not a valid XNB file");

		byte target = m_stream->ReadByte();
		if (target == 'w')
			m_target = TargetPlatform_Windows;
		else if (target == 'm')
			m_target = TargetPlatform_WinPhone;
		else if (target == 'x')
			m_target = TargetPlatform_XBox360;
		else
			throw ContentException("Not a valid XNB file");

		byte version = m_stream->ReadByte();
		if (version != 5)
			throw ContentException("Not a valid XNB file");

		byte flags = m_stream->ReadByte();
		m_isHighDef = flags & 0x01;
		m_isCompressed = flags & 0x80;

		m_compressedSize = m_stream->ReadInt32();
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
		
		m_stream->Advance(len);
	}
}
}