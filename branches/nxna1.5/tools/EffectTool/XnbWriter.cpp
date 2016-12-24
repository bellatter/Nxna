#include "XnbWriter.h"

XnbWriter::XnbWriter(ResizableBuffer* buffer)
{
	m_buffer = buffer;

	// write the XNB header
	char header[] = {'X', 'N', 'B'};
	m_buffer->Write(header, 3);

	char targetPlatform = 'w'; // windows
	m_buffer->Write(&targetPlatform, 1);

	char version = 5;
	m_buffer->Write(&version, 1);

	char flags = 0;
	m_buffer->Write(&flags, 1);

	// write size
	unsigned int size = 0;
	m_buffer->Write((char*)&size, 4);

	// write # readers
	char numReaders = 1; // this is technically supposed to be a 7-bit encoded int
	m_buffer->Write((char*)&numReaders, 1);

	// write the reader
	const char* readerName = "NXNAEffectReader";
	char readerNameLen = strlen(readerName);
	m_buffer->Write(&readerNameLen, 1);
	m_buffer->Write(readerName, readerNameLen);
	
	unsigned int readerVersion = 0;
	m_buffer->Write((char*)&readerVersion, 4);

	char sharedResourceCount = 0;
	m_buffer->Write(&sharedResourceCount, 1);
}

void XnbWriter::Finalize()
{
	const int sizeOffset = 6;

	unsigned int size = m_buffer->GetSize();
	m_buffer->WriteUIntAt(size, sizeOffset);
}