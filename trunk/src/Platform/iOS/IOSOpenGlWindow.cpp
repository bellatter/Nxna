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

	void IOSOpenGlWindow::SetScreenSize(const Nxna::Graphics::PresentationParameters& pp)
	{
		int width, height;
		IOSGame_GetScreenSize(&width, &height);

		Nxna::Graphics::PresentationParameters newParams = pp;
		newParams.BackBufferWidth = width;
		newParams.BackBufferHeight = height;
		newParams.BackBufferFormat = Graphics::SurfaceFormat::Color;
		newParams.DepthStencilFormat = Graphics::DepthFormat::Depth24Stencil8;
		newParams.IsFullScreen = true;
		newParams.MultiSampleCount = 0;
		
		static_cast<Nxna::Graphics::OpenGl::OpenGlDevice*>(m_device)->OnContextCreated();
		static_cast<Nxna::Graphics::OpenGl::OpenGlDevice*>(m_device)->UpdatePresentationParameters(newParams);
		
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