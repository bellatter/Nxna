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

// Determine the platform

#ifdef __native_client__
// the Google Native Client
#define NXNA_PLATFORM_NACL

#elif defined _WIN32
// Windows
#define NXNA_PLATFORM_WIN32

#elif defined __APPLE__
// some kind of Apple platform
#define NXNA_PLATFORM_APPLE
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE == 0
#define NXNA_PLATFORM_APPLE_OSX
#else
#define NXNA_PLATFORM_APPLE_IOS
#endif

#elif defined __ANDROID__
// Android
#define NXNA_PLATFORM_ANDROID

#else
// assume Linux
#define NXNA_PLATFORM_NIX

#endif



#endif // NXNA_COMMON_H
