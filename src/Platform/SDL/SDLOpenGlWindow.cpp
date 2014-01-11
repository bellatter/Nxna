#include <cassert>
#include "SDLOpenGlWindow.h"
#include "../../Graphics/OpenGL/OpenGLDevice.h"
#include "../../Input/Touch/TouchPanel.h"

#if defined NXNA_PLATFORM_APPLE
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif

namespace Nxna
{
namespace Platform
{
namespace SDL
{
	SDLOpenGlWindow::SDLOpenGlWindow(Nxna::Game* game)
		: Nxna::GraphicsDeviceManager(game)
	{
	}

	Nxna::Graphics::GraphicsDevice* SDLOpenGlWindow::CreateDevice()
	{
		m_device = new Nxna::Graphics::OpenGl::OpenGlDevice();

		return m_device;
	}

	void SDLOpenGlWindow::BeginDraw()
	{
	}

	void SDLOpenGlWindow::EndDraw()
	{
		SDL_GL_SwapWindow((SDL_Window*)m_window);
	}

	void SDLOpenGlWindow::ApplyChanges()
	{
	}

	void SDLOpenGlWindow::ShowWindow()
	{
	}

	void SDLOpenGlWindow::DestroyWindow()
	{
		SDL_GL_DeleteContext((SDL_GLContext)m_glContext);
		SDL_DestroyWindow((SDL_Window*)m_window);
		SDL_Quit();
	}

	Graphics::SurfaceFormat convertSDLFormat(unsigned int format);

	void SDLOpenGlWindow::SetScreenSize(const Nxna::Graphics::PresentationParameters& pp)
	{
		assert(m_device != nullptr);

        SDL_SetHint( "SDL_HINT_ORIENTATIONS", "LandscapeLeft LandscapeRight" );

        PreferredBackBufferWidth(pp.BackBufferWidth);
		PreferredBackBufferHeight(pp.BackBufferHeight);

		if (pp.BackBufferFormat == Graphics::SurfaceFormat::Bgr565)
		{
			SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5);
			SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 6);
			SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5);
		}
		else if (pp.BackBufferFormat == Graphics::SurfaceFormat::Color)
		{
			SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8);
			SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8);
			SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8);
		}

		if (pp.DepthStencilFormat == Graphics::DepthFormat::Depth16)
		{
			SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16);
		}
		else if (pp.DepthStencilFormat == Graphics::DepthFormat::Depth24)
		{
			SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24);
			SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		}
		else if (pp.DepthStencilFormat == Graphics::DepthFormat::Depth24Stencil8)
		{
			SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24);
			SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		}

		if (pp.MultiSampleCount > 0)
		{
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, pp.MultiSampleCount);
		}

		SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
        
        int flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

		if (pp.IsFullScreen)
			flags |= SDL_WINDOW_FULLSCREEN;
        
		m_window = SDL_CreateWindow("CNK", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
			PreferredBackBufferWidth(), PreferredBackBufferHeight(), flags);

		if (m_window == nullptr)
			throw Exception(SDL_GetError());
        
		int width, height;
        SDL_GetWindowSize((SDL_Window*)m_window, &width, &height);
        
        PreferredBackBufferWidth(width);
        PreferredBackBufferHeight(height);
        
		m_glContext = SDL_GL_CreateContext((SDL_Window*)m_window);

		static_cast<Nxna::Graphics::OpenGl::OpenGlDevice*>(m_device)->OnContextCreated();

		Graphics::PresentationParameters newParams = pp;
		newParams.BackBufferWidth = PreferredBackBufferWidth();
		newParams.BackBufferHeight = PreferredBackBufferHeight();

		static_cast<Nxna::Graphics::OpenGl::OpenGlDevice*>(m_device)->UpdatePresentationParameters(newParams);

		m_device->SetViewport(Nxna::Graphics::Viewport(0, 0, PreferredBackBufferWidth(), PreferredBackBufferHeight()));

		// tell the touch panel the display size
		Input::Touch::TouchPanel::SetDisplayWidth(width);
		Input::Touch::TouchPanel::SetDisplayHeight(height);
	}

	Graphics::SurfaceFormat convertSDLFormat(unsigned int format)
	{
		switch(format)
		{
		case SDL_PIXELFORMAT_BGR565:
			return Graphics::SurfaceFormat::Bgr565;
		case SDL_PIXELFORMAT_BGRA4444:
			return Graphics::SurfaceFormat::Bgra4444;
		case SDL_PIXELFORMAT_BGRA5551:
			return Graphics::SurfaceFormat::Bgra5551;
		case SDL_PIXELFORMAT_ARGB8888:
			return Graphics::SurfaceFormat::Color;
		default:
			throw Graphics::GraphicsException("Unknown SDL surface format");
		}
	}
}
}
}