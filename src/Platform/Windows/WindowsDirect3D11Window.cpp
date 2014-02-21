#include "../../NxnaConfig.h"

#if !defined NXNA_DISABLE_D3D11

#include <cassert>
#include "WindowsDirect3D11Window.h"
#include "../../Graphics/Direct3D11/Direct3D11Device.h"
#include "../../Input/Touch/TouchPanel.h"
#include "WindowsCommon.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>


namespace Nxna
{
namespace Platform
{
namespace Windows
{
	WindowsDirect3D11Window::WindowsDirect3D11Window(Nxna::Game* game)
		: Nxna::GraphicsDeviceManager(game)
	{
		
	}

	Nxna::Graphics::GraphicsDevice* WindowsDirect3D11Window::CreateDevice()
	{
		m_device = new Nxna::Graphics::Direct3D11::Direct3D11Device();

		return m_device;
	}

	void WindowsDirect3D11Window::BeginDraw()
	{
	}

	void WindowsDirect3D11Window::EndDraw()
	{
		m_device->Present();
	}

	void WindowsDirect3D11Window::ApplyChanges()
	{
	}

	void WindowsDirect3D11Window::ShowWindow()
	{
	}

	void WindowsDirect3D11Window::DestroyWindow()
	{
	}

	void WindowsDirect3D11Window::SetScreenSize(const Graphics::PresentationParameters& pp)
	{
		assert(m_device != nullptr);

		m_window = CreateGameWindow(pp.BackBufferWidth, pp.BackBufferHeight, pp.IsFullScreen);

		::ShowWindow((HWND)m_window, SW_SHOW);
		SetForegroundWindow((HWND)m_window);
		SetFocus((HWND)m_window);

        PreferredBackBufferWidth(pp.BackBufferWidth);
        PreferredBackBufferHeight(pp.BackBufferHeight);

		static_cast<Nxna::Graphics::Direct3D11::Direct3D11Device*>(m_device)->OnWindowCreated(m_window, pp);

		m_device->SetViewport(Nxna::Graphics::Viewport(0, 0, PreferredBackBufferWidth(), PreferredBackBufferHeight()));

		// tell the touch panel the display size
		Input::Touch::TouchPanel::SetDisplayWidth(pp.BackBufferWidth);
		Input::Touch::TouchPanel::SetDisplayHeight(pp.BackBufferHeight);
	}

	
}
}
}

#endif
