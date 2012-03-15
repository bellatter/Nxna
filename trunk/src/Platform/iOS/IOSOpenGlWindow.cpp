#include "IOSOpenGlWindow.h"
#include "IOSGame_c.h"
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
		IOSGame_GetScreenSize(&width, &height);
		
		static_cast<Nxna::Graphics::OpenGl::OpenGlDevice*>(m_device)->OnContextCreated();
		static_cast<Nxna::Graphics::OpenGl::OpenGlDevice*>(m_device)->UpdateScreenSize(width, height);
		
		m_device->SetViewport(Nxna::Graphics::Viewport(0, 0, width, height));
	}
	
	void IOSOpenGlWindow::ShowLeaderboard(const char* category, LeaderboardTimeScope scope)
	{
		IOSGame_ShowLeaderboard(category, (int)scope);
	}
	
	void IOSOpenGlWindow::ShowAchievements()
	{
		IOSGame_ShowAchievements();
	}
}
}
}