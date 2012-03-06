#include "IOSOpenGlWindow.h"
#include "../../Graphics/OpenGL/OpenGLDevice.h"
#include "../../Input/Touch/TouchPanel.h"

namespace Nxna
{
namespace Platform
{
namespace iOS
{
	IOSOpenGlWindow::IOSOpenGlWindow(Nxna::Game* game)
		: Nxna::GraphicsDeviceManager(game)
	{
		
	}

	Nxna::Graphics::GraphicsDevice* IOSOpenGlWindow::CreateDevice()
	{
		m_device = new Nxna::Graphics::OpenGl::OpenGlDevice();

		return m_device;
	}

	void IOSOpenGlWindow::BeginDraw()
	{
	}

	void IOSOpenGlWindow::EndDraw()
	{
	}

	void IOSOpenGlWindow::ApplyChanges()
	{
	}

	void IOSOpenGlWindow::ShowWindow()
	{
	}

	void IOSOpenGlWindow::DestroyWindow()
	{
	}

	void IOSOpenGlWindow::SetScreenSize(int width, int height, bool fullscreen)
	{
	}
}
}
}