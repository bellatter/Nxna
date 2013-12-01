#include <cassert>
#include "SDLDirect3D11Window.h"
#include "../../Graphics/Direct3D11/Direct3D11Device.h"
#include "../../Input/Touch/TouchPanel.h"
#include <SDL.h>
#include <SDL_syswm.h>

namespace Nxna
{
namespace Platform
{
namespace SDL
{
	SDLDirect3D11Window::SDLDirect3D11Window(Nxna::Game* game)
		: Nxna::GraphicsDeviceManager(game)
	{
	}

	Nxna::Graphics::GraphicsDevice* SDLDirect3D11Window::CreateDevice()
	{
		m_device = new Nxna::Graphics::Direct3D11::Direct3D11Device();

		return m_device;
	}

	void SDLDirect3D11Window::BeginDraw()
	{
	}

	void SDLDirect3D11Window::EndDraw()
	{
		m_device->Present();
	}

	void SDLDirect3D11Window::ApplyChanges()
	{
	}

	void SDLDirect3D11Window::ShowWindow()
	{
	}

	void SDLDirect3D11Window::DestroyWindow()
	{
		SDL_DestroyWindow((SDL_Window*)m_window);
		SDL_Quit();
	}

	void SDLDirect3D11Window::SetScreenSize(int width, int height, bool fullscreen)
	{
		assert(m_device != nullptr);

        SDL_SetHint( "SDL_HINT_ORIENTATIONS", "LandscapeLeft LandscapeRight" );

        PreferredBackBufferWidth(width);
		PreferredBackBufferHeight(height);
        
        int flags = SDL_WINDOW_SHOWN;

		if (fullscreen)
			flags |= SDL_WINDOW_FULLSCREEN;
        
		m_window = SDL_CreateWindow("CNK", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
			PreferredBackBufferWidth(), PreferredBackBufferHeight(), flags);

		if (m_window == nullptr)
			throw Exception(SDL_GetError());
        
        SDL_GetWindowSize((SDL_Window*)m_window, &width, &height);
        
        PreferredBackBufferWidth(width);
        PreferredBackBufferHeight(height);
       
		SDL_SysWMinfo info;
		SDL_VERSION(&info.version);
		SDL_GetWindowWMInfo((SDL_Window*)m_window, &info);
		static_cast<Nxna::Graphics::Direct3D11::Direct3D11Device*>(m_device)->OnWindowCreated(info.info.win.window, PreferredBackBufferWidth(), PreferredBackBufferHeight());

		m_device->SetViewport(Nxna::Graphics::Viewport(0, 0, PreferredBackBufferWidth(), PreferredBackBufferHeight()));

		// tell the touch panel the display size
		Input::Touch::TouchPanel::SetDisplayWidth(width);
		Input::Touch::TouchPanel::SetDisplayHeight(height);
	}
}
}
}
