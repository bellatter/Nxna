#ifndef NXNA_COMMON_H
#define NXNA_COMMON_H

#ifndef NDEBUG
#include <cassert>
#endif

namespace Nxna
{
	typedef unsigned char nxna_byte;

	enum class NxnaResult
	{
		/// Success, no errors
		Success = 0,

		/// The requested operation is not supported on the current platform or hardware
		NotSupported = -10000,

		InvalidArgument = -10100,

		/// An unknown error
		UnknownError = -1
	};

	struct NxnaResultDetails
	{
		const char* Filename;
		int LineNumber;
		
		int APIErrorCode;
		char ErrorDescription[128];
	};
}

#ifndef NXNA_DISABLE_VALIDATION
#define NXNA_VALIDATION_ASSERT(c, t) assert((c) && t)
#else
#define NXNA_VALIDATION_ASSERT(c, t)
#endif

#endif // NXNA_COMMON_H
