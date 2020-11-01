/*

Defining each of these changes how NXNA is built

NXNA_ENABLE_MATH - enabled Vector2, Vector3, and Matrix classes
NXNA_ENABLE_RENDERER - enables GraphicsDevice and all the related classes
NXNA_ENABLE_INPUT - enables the Input related classes

NXNA_ENABLE_DIRECT3D11 - Enables the Direct3D11 renderer. (no effect on non-Windows platforms)

NXNA_DISABLE_VALIDATION - Disables some extra run-time checks

NXNA_DISABLE_GLEW_IMPLEMENTATION - Don't include glew.c when building Nxna

*/

/*! \mainpage Nxna2

\section intro Introduction

Nxna2 is primarily a cross-platform wrapper around OpenGL and Direct3D. Write
your graphics code once with Nxna2 and get maximum portability!

\section compiling Compiling

Compiling Nxna2 is simple. The easiest thing to do is this:
 -# Copy all the Nxna2 source to somewhere that your compiler can see it (perhaps within your code's project directory?)
 -# Create an empty .h and matching empty .cpp file. Add the .cpp file to your build. 
 -# Within the .h file you created, enable the features you want (using #define), and at the very end #include the Nxna2.h file. For example:
 \code
 #define NXNA_ENABLE_MATH
 #define NXNA_ENABLE_RENDERER
 #define NXNA_ENABLE_INPUT
 #define NXNA_ENABLE_DIRECT3D11
 #include "Nxna2.h"
 \endcode
 -# Within the .cpp file add the following lines (be sure to modify the #include)
  \code
  #define NXNA2_IMPLEMENTATION
  #include "NameOfHeaderYouCreatedInStep2GoesHere.h"
  \endcode

Now whenever you need to use Nxna2, just #include the header file you created.

\section startup Initialization

The first thing to do is to create a new window, along with an OpenGL context if you
intend to use the OpenGL renderer or a Direct3D device and device context if you intend to
use Direct3D. It's your responsibility to set up the window and context/device. Nxna2 won't do it.
That's so that you have complete control over the window creation process without Nxna2 getting
in the way.

Next, you'll need to create a new GraphicsDevice object, which will act as a wrapper around whatever API
you choose. You do that by filling out an Nxna::Graphics::GraphicsDeviceDesc struct and pass that to
Nxna::Graphics::GraphicsDevice::CreateGraphicsDevice().

To create a new OpenGL device:
\code
Nxna::Graphics::GraphicsDevice device;
Nxna::Graphics::GraphicsDeviceDesc params;
memset(&params, 0, sizeof(Nxna::Graphics::GraphicsDeviceDesc));
params.Type = Nxna::Graphics::GraphicsDeviceType::OpenGl41;
params.ScreenWidth = 640;
params.ScreenHeight = 480;
Nxna::Graphics::GraphicsDevice::CreateGraphicsDevice(&params, &device);
\endcode

To create a Direct3D 11 device:
\code
Nxna::Graphics::GraphicsDevice device;
Nxna::Graphics::GraphicsDeviceDesc params;
memset(&params, 0, sizeof(Nxna::Graphics::GraphicsDeviceDesc));
params.Type = Nxna::Graphics::GraphicsDeviceType::Direct3D11;
params.ScreenWidth = 640;
params.ScreenHeight = 480;
params.Direct3D11.Device = deviceThatYouCreatedInPreviousStep;
params.Direct3D11.DeviceContext = contextThatYouCreatedInPreviousStep;
params.Direct3D11.RenderTargetView = rtvThatYouAlreadyCreated;
params.Direct3D11.SwapChain = swapChainYouAlreadyCreated;
Nxna::Graphics::GraphicsDevice::CreateGraphicsDevice(&params, &device);
\endcode

To be continued...

Basically, you just create a bunch of state objects by passing ...Desc to GraphicsDevice::Create...(), then call
GraphicsDevice::Set...() with that new object. Then you create your index buffers and vertex buffer and constant buffers,
set those two, and then render.

\section stuff Other random stuff

Nxna2 has lots of objects, like BlendState, VertexBuffer, etc, that have unions with different OpenGL and Direct3D references in them.
Generally you should treat those objects as completely opaque and not mess around with what is inside. But if you want to, go for it.

*/

#ifndef NXNA2_H
#define NXNA2_H

// do some sanity checks
#ifndef _WIN32
#ifdef NXNA_ENABLE_DIRECT3D11
#undef NXNA_ENABLE_DIRECT3D11
#endif
#endif


#include "NxnaCommon.h"


#ifdef NXNA_ENABLE_MATH

namespace Nxna
{
	struct Point
	{
		int X, Y;
	};
}

#include "MathHelper.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix.h"
#include "Quaternion.h"
#endif

#include "Rectangle.h"

#ifdef NXNA_ENABLE_RENDERER
#include "Color.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/PipelineState.h"
#else
#ifdef NXNA_ENABLE_VIEWPORT
#include "Graphics/Viewport.h"
#endif
#endif

#ifdef NXNA_ENABLE_INPUT
#include "Input/InputState.h"
#endif

#ifdef NXNA_ENABLE_SPRITEBATCH
#ifndef NXNA_ENABLE_RENDERER
#error NXNA_ENABLE_SPRITEBATCH requires NXNA_ENABLE_RENDERER
#endif
#include "Graphics/SpriteBatch.h"
#endif

#ifdef NXNA_ENABLE_SPRITEFONT
#ifndef NXNA_ENABLE_SPRITEBATCH
#error NXNA_ENABLE_SPRITEFONT requries NXNA_ENABLE_SPRITEBATCH
#endif
#include "Graphics/SpriteFont.h"
#endif

#endif // NXNA2_H

#ifdef NXNA2_IMPLEMENTATION

#ifndef NXNA_MEMSET
#define NXNA_MEMSET memset
#endif

#ifndef NXNA_MEMCPY
#define NXNA_MEMCPY memcpy
#endif

#ifndef _WIN32
#ifdef NXNA_ENABLE_DIRECT3D11
#error NXNA_ENABLE_DIRECT3D11 is not valid on any platform other than Windows
#define NXNA_ERROR_STOP_COMPILATION
#endif
#endif

#ifndef NXNA_ERROR_STOP_COMPILATION
#ifdef NXNA_ENABLE_MATH
#include "MathHelper.cpp"
#include "Vector2.cpp"
#include "Vector3.cpp"
#include "Vector4.cpp"
#include "Matrix.cpp"
#include "Quaternion.cpp"
#endif

#ifdef NXNA_ENABLE_SPRITEBATCH
#include "Graphics/SpriteBatch.cpp"
#endif

#ifdef NXNA_ENABLE_RENDERER
#include "Graphics/GraphicsDevice.cpp"
#include "Graphics/OpenGL.cpp"
#include "Graphics/PipelineState.cpp"
#include "Graphics/Viewport.cpp"
#else
#ifdef NXNA_ENABLE_VIEWPORT
#include "Graphics/Viewport.cpp"
#endif
#endif


#endif // NXNA_ERROR_STOP_COMPILATION


#endif // NXNA2_IMPLEMENTATION
