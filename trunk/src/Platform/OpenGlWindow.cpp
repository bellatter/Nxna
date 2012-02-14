#include <cassert>
#include "PlatformDefs.h"
#include "OpenGlWindow.h"
#include "../Graphics/OpenGL/OpenGLDevice.h"
#include "../Exception.h"

#ifdef NXNA_PLATFORM_NACL
#include <ppapi/cpp/instance.h>
#include <ppapi/cpp/graphics_3d_client.h>
#include <ppapi/cpp/graphics_3d.h>
#include <ppapi/gles2/gl2ext_ppapi.h>

extern pp::Instance* g_Instance;
#else
#include <SDL.h>
#endif

namespace Nxna
{
namespace Platform
{
#ifdef NXNA_PLATFORM_NACL
	class OpenGlContext : public pp::Graphics3DClient
	{
		pp::Graphics3D m_context;

	public:

		explicit OpenGlContext(pp::Instance* instance, int width, int height)
			: pp::Graphics3DClient(instance)
		{
			int attribs[] = 
			{
				PP_GRAPHICS3DATTRIB_ALPHA_SIZE, 8,
				PP_GRAPHICS3DATTRIB_DEPTH_SIZE, 24,
				PP_GRAPHICS3DATTRIB_STENCIL_SIZE, 8,
				PP_GRAPHICS3DATTRIB_SAMPLES, 0,
				PP_GRAPHICS3DATTRIB_SAMPLE_BUFFERS, 0,
				PP_GRAPHICS3DATTRIB_WIDTH, width,
				PP_GRAPHICS3DATTRIB_HEIGHT, height,
				PP_GRAPHICS3DATTRIB_NONE
			};

			m_context = pp::Graphics3D(*instance, pp::Graphics3D(), attribs);

			instance->BindGraphics(m_context);
		}

		virtual ~OpenGlContext()
		{
			glSetCurrentContextPPAPI(0);
		}

		virtual void Graphics3DContextLost()
		{
    			assert(!"Unexpectedly lost graphics context");
  		}
	};

#endif


	OpenGlWindow::OpenGlWindow(Nxna::Game* game)
		: Nxna::GraphicsDeviceManager(game)
	{
		
	}

	Nxna::Graphics::GraphicsDevice* OpenGlWindow::CreateDevice()
	{
		m_device = new Nxna::Graphics::OpenGl::OpenGlDevice();

		return m_device;
	}

	void OpenGlWindow::BeginDraw()
	{
	}

	void OpenGlWindow::EndDraw()
	{
#ifndef NXNA_PLATFORM_NACL
		SDL_GL_SwapWindow((SDL_Window*)m_window);
#endif
	}

	void OpenGlWindow::ApplyChanges()
	{
	}

	void OpenGlWindow::ShowWindow()
	{
	}

	void OpenGlWindow::DestroyWindow()
	{
#ifndef NXNA_PLATFORM_NACL
		SDL_GL_DeleteContext((SDL_GLContext)m_glContext);
		SDL_DestroyWindow((SDL_Window*)m_window);
		SDL_Quit();
#endif
	}

#ifdef NXNA_PLATFORM_APPLE_IOS
	bool g_isRetinaDisplay = false;
#endif

	void OpenGlWindow::SetScreenSize(int width, int height, bool fullscreen)
	{
#ifndef NXNA_PLATFORM_NACL
		
		SDL_Init(SDL_INIT_VIDEO);
        SDL_SetHint( "SDL_HINT_ORIENTATIONS", "LandscapeLeft LandscapeRight" );
		
				
#ifdef NXNA_PLATFORM_APPLE_IOS
		if (fullscreen)
		{
			// figure out what the "fullscreen" resolution is
			SDL_DisplayMode bestMode;
			bestMode.w = bestMode.h = 0;
			int modes = SDL_GetNumDisplayModes(0);
			for (int i = 0; i < modes; i++)
			{
				SDL_DisplayMode mode;
				SDL_GetDisplayMode(0, i, &mode);
				
				if (mode.w > bestMode.w && mode.h > bestMode.h)
					bestMode = mode;
			}
			
			width = bestMode.w;
			height = bestMode.h;

			if (width == 960 || height == 960)
				g_isRetinaDisplay = true;
		}
#endif
		
		
        PreferredBackBufferWidth(width);
		PreferredBackBufferHeight(height);

		int bitsPerPixel = 32;

		if (bitsPerPixel == 16)
		{
			SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5);
			SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 6);
			SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5);
			SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16);
		}
		else
		{
			SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8);
			SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8);
			SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8);
			SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24);
			SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		}
		SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        
        int flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
#ifdef NXNA_PLATFORM_APPLE_IOS
        flags |= SDL_WINDOW_BORDERLESS;
#endif
		if (fullscreen)
			flags |= SDL_FULLSCREEN;
        
		m_window = SDL_CreateWindow("CNK", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
			PreferredBackBufferWidth(), PreferredBackBufferHeight(), flags);

		if (m_window == nullptr)
			throw Exception(SDL_GetError());
        
#ifndef NXNA_PLATFORM_APPLE_IOS
        SDL_GetWindowSize((SDL_Window*)m_window, &width, &height);
        
        PreferredBackBufferWidth(width);
        PreferredBackBufferHeight(height);
#endif
        
		m_glContext = SDL_GL_CreateContext((SDL_Window*)m_window);

		static_cast<Nxna::Graphics::OpenGl::OpenGlDevice*>(m_device)->OnContextCreated();
		static_cast<Nxna::Graphics::OpenGl::OpenGlDevice*>(m_device)->UpdateScreenSize(PreferredBackBufferWidth(), PreferredBackBufferHeight());

		m_device->SetViewport(Nxna::Graphics::Viewport(0, 0, PreferredBackBufferWidth(), PreferredBackBufferHeight()));
#else
		OpenGlContext* context = new OpenGlContext(g_Instance, width, height);
#endif
	}
}
}
