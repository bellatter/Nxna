#ifndef NXNA_GRAPHICS_OPENGL_OPENGL_H
#define NXNA_GRAPHICS_OPENGL_OPENGL_H

#include "../../Platform/PlatformDefs.h"

#ifdef USING_OPENGLES
#ifdef NXNA_PLATFORM_NACL
#include <GLES2/gl2.h>
#else
#include <OpenGLES/ES2/gl.h>
#endif
#else
#include <GL/glew.h>
#endif

#endif // NXNA_GRAPHICS_OPENGL_OPENGL_H