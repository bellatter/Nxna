#ifndef NXNA_CONTENT_MAPPEDFILESTREAM_H
#define NXNA_CONTENT_MAPPEDFILESTREAM_H

#include "FileStream.h"

namespace Nxna
{
namespace Content
{
	class MappedFileStream : public MemoryStream
	{
#ifdef _WIN32
		void* m_fp;
#else
		int m_fd;
#endif
		void* m_mapping;

	public:
		MappedFileStream();
		MappedFileStream(const char* path);
		virtual ~MappedFileStream();

		virtual bool IsOpen();
		static bool Load(const char* path, MappedFileStream* result);
	};
}
}

#endif // NXNA_CONTENT_MAPPEDFILESTREAM_H
