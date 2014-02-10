#ifndef CONTENT_FILESTREAM_H
#define CONTENT_FILESTREAM_H

#include "../NxnaConfig.h"

NXNA_DISABLE_OVERRIDE_WARNING

namespace Nxna
{
namespace Content
{
	NXNA_ENUM(SeekOrigin)
		Begin,
		Current,
		End
	END_NXNA_ENUM(SeekOrigin)

	class Stream
	{
	public:
		virtual ~Stream() { }

		virtual int Read(byte* destination, int length) = 0;
		virtual int ReadInt32() = 0;
		virtual short ReadInt16() = 0;
		virtual float ReadFloat() = 0;
		virtual byte ReadByte() = 0;

		virtual void Seek(int offset, SeekOrigin origin) = 0;

		virtual int Position() = 0;
		virtual int Length() = 0;
	};

	class FileStream : public Stream
	{
		void* m_fp;

	protected:
		int m_bytesRead;

	public:
		FileStream(const char* path);
		virtual ~FileStream();

		virtual bool IsOpen();
		virtual int Read(byte* destination, int length) override;
		virtual int ReadInt32() override;
		virtual short ReadInt16() override;
		virtual float ReadFloat() override;
		virtual byte ReadByte() override;

		virtual void Seek(int offset, SeekOrigin origin) override;
		void Advance(int bytes) { Seek(bytes, SeekOrigin::Current); }

		virtual int Position() override;
		virtual int Length() override;
		bool Eof();

	protected:
		FileStream();

	private:
		void swapLE(void* data, int length);
	};

	class MemoryStream : public Stream
	{
		const byte* m_memory;
		int m_length;
		int m_position;

	public:
		MemoryStream(const byte* memory, int length);
		virtual ~MemoryStream() { }

		virtual int Read(byte* destination, int length) override;
		virtual int ReadInt32() override;
		virtual short ReadInt16() override;
		virtual float ReadFloat() override;
		virtual byte ReadByte() override;

		virtual void Seek(int offset, SeekOrigin origin) override;

		virtual int Position() override;
		virtual int Length() override;
		bool Eof();

	private:
		void swapLE(void* data, int length);
	};
}
}

NXNA_ENABLE_OVERRIDE_WARNING

#endif 