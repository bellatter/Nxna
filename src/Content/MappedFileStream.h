#ifndef NXNA_CONTENT_MAPPEDFILESTREAM_H
#define NXNA_CONTENT_MAPPEDFILESTREAM_H

#include "FileStream.h"

namespace Nxna
{
namespace Content
{
	class MappedFileStream : public Stream
	{
#ifdef _WIN32
		void* m_fp;
#else
		int m_fd;
#endif
		void* m_mapping;
		void* m_baseAddress;
		int m_size;

	protected:
		int m_bytesRead;

	public:
		MappedFileStream();
		MappedFileStream(const char* path);
		virtual ~MappedFileStream();

		void Load(const char* path);

		virtual bool IsOpen();
		virtual int Read(byte* destination, int length) override;
		virtual int ReadInt32() override;
		virtual short ReadInt16() override;
		virtual float ReadFloat() override;
		virtual byte ReadByte() override;

		virtual void Seek(int offset, SeekOrigin origin) override;
		void Advance(int bytes) { Seek(bytes, SeekOrigin::Current); }

		virtual int Position() override { return m_bytesRead; }
		virtual int Length() override { return m_size; }
		bool Eof() { return m_bytesRead >= m_size; }

		void* GetData() { return m_baseAddress; }

	protected:
		

	private:
		void swapLE(void* data, int length);
	};
}
}

#endif // NXNA_CONTENT_MAPPEDFILESTREAM_H
