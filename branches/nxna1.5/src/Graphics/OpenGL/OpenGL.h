#ifndef NXNA_GRAPHICS_OPENGL_OPENGL_H
#define NXNA_GRAPHICS_OPENGL_OPENGL_H

#include "../../NxnaConfig.h"

#ifdef USING_OPENGLES
#if defined NXNA_PLATFORM_NACL || defined NXNA_PLATFORM_ANDROID
#include <GLES2/gl2.h>
#else
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#endif
#else
#include <GL/glew.h> 
#endif

#endif // NXNA_GRAPHICS_OPENGL_OPENGL_H
