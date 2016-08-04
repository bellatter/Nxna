#include "OpenGL.h"

#if 0
#undef NXNA_DECLARE_GL_EXTENSION
#define NXNA_DECLARE_GL_EXTENSION(vmajor, vminor, p, n) p n;
#include "GLExtensions.h"

/* OpenGL 4.3 */
PFNGLDEBUGMESSAGECALLBACKPROC glDebugMessageCallback;
#endif

namespace Nxna
{
namespace Graphics
{
namespace OpenGL
{
	void LoadGLExtensions(int glMajorVersion, int glMinorVersion)
	{
#if 0
#define GL_LOAD_PROC(t, p) p = (t)wglGetProcAddress(#p)
#undef NXNA_DECLARE_GL_EXTENSION
#define NXNA_DECLARE_GL_EXTENSION(vmajor, vminor, p, n) n = nullptr;
#include "GLExtensions.h"

#define NXNA_GL_LOAD_PROC

		if (glMajorVersion >= 2)
		{
#define NXNA_DECLARE_EXTENSIONS_20
#include "GLExtensions.h"
		}

		if (glMajorVersion >= 3)
		{
#define NXNA_DECLARE_EXTENSIONS_30
#include "GLExtensions.h"

			if (glMajorVersion > 3 || glMinorVersion >= 1)
			{
#define NXNA_DECLARE_EXTENSIONS_31
#include "GLExtensions.h"
			}

			if (glMajorVersion > 3 || glMinorVersion >= 2)
			{
#define NXNA_DECLARE_EXTENSIONS_32
#include "GLExtensions.h"
			}
		}

		if (allowAllExtensions || glMajorVersion >= 4)
		{
#define NXNA_DECLARE_EXTENSIONS_40
#include "GLExtensions.h"

			if (allowAllExtensions || glMinorVersion > 4 || glMinorVersion >= 1)
			{
#define NXNA_DECLARE_EXTENSIONS_41
#include "GLExtensions.h"

			}

			if (allowAllExtensions || glMinorVersion > 4 || glMinorVersion >= 2)
			{
#define NXNA_DECLARE_EXTENSIONS_42
#include "GLExtensions.h"
			}

			if (allowAllExtensions || glMajorVersion > 4 || glMinorVersion >= 5)
			{
#define NXNA_DECLARE_EXTENSIONS_45
#include "GLExtensions.h"
			}
		}

		// always attempt to load these functions, no matter the context version
		GL_LOAD_PROC(PFNGLDEBUGMESSAGECALLBACKPROC, glDebugMessageCallback);
#else
		glewInit();
#endif
	}
}
}
}

#define GLEW_INCLUDE "glew.h"
#define GLEW_WGLEW_INCLUDE "wglew.h"
#define GLEW_GLXEW_INCLUDE "glxew.h"
#include "glew/glew.c"
