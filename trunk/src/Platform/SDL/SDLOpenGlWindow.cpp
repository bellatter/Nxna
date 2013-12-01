#include <cassert>
#include "SDLOpenGlWindow.h"
#include "../../Graphics/OpenGL/OpenGLDevice.h"
#include "../../Input/Touch/TouchPanel.h"
#include <SDL.h>

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

	void SDLOpenGlWindow::SetScreenSize(int width, int height, bool fullscreen)
	{
		assert(m_device != nullptr);

        SDL_SetHint( "SDL_HINT_ORIENTATIONS", "LandscapeLeft LandscapeRight" );

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

		if (fullscreen)
			flags |= SDL_WINDOW_FULLSCREEN;
        
		m_window = SDL_CreateWindow("CNK", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
			PreferredBackBufferWidth(), PreferredBackBufferHeight(), flags);

		if (m_window == nullptr)
			throw Exception(SDL_GetError());
        
        SDL_GetWindowSize((SDL_Window*)m_window, &width, &height);
        
        PreferredBackBufferWidth(width);
        PreferredBackBufferHeight(height);
        
		m_glContext = SDL_GL_CreateContext((SDL_Window*)m_window);

		static_cast<Nxna::Graphics::OpenGl::OpenGlDevice*>(m_device)->OnContextCreated();
		static_cast<Nxna::Graphics::OpenGl::OpenGlDevice*>(m_device)->UpdateScreenSize(PreferredBackBufferWidth(), PreferredBackBufferHeight());

		m_device->SetViewport(Nxna::Graphics::Viewport(0, 0, PreferredBackBufferWidth(), PreferredBackBufferHeight()));

		// tell the touch panel the display size
		Input::Touch::TouchPanel::SetDisplayWidth(width);
		Input::Touch::TouchPanel::SetDisplayHeight(height);
	}
}
}
}