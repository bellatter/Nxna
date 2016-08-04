#ifndef NXNA_GRAPHICS_OPENGL_H
#define NXNA_GRAPHICS_OPENGL_H

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#if 0
#include <GL/gl.h>
#include "glext.h"


// create function prototypes for everything that Windows leaves out

#define NXNA_DECLARE_GL_EXTENSION_20(p, n) NXNA_DECLARE_GL_EXTENSION(2, 0, p, n)
#define NXNA_DECLARE_GL_EXTENSION_30(p, n) NXNA_DECLARE_GL_EXTENSION(3, 0, p, n)
#define NXNA_DECLARE_GL_EXTENSION(vmajor, vminor, p,n) extern p n;
#include "GLExtensions.h"

/* OpenGL 4.3 */
extern PFNGLDEBUGMESSAGECALLBACKPROC glDebugMessageCallback;
#else

#define GLEW_STATIC
#include "glew/glew.h"

#endif

namespace Nxna
{
namespace Graphics
{
namespace OpenGL
{
	void LoadGLExtensions(int glMajorVersion, int glMinorVersion, bool allowAllExtensions);
}
}
}

#endif // NXNA_GRAPHICS_OPENGL_H

