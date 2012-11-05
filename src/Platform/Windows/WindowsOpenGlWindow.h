#ifndef NXNA_PLATFORM_WINDOWS_WINDOWSOPENGLWINDOW_H
#define NXNA_PLATFORM_WINDOWS_WINDOWSOPENGLWINDOW_H

#include "../../NxnaConfig.h"
#include "../../IGraphicsDeviceManager.h"

namespace Nxna
{
namespace Platform
{
namespace Windows
{
	class WindowsOpenGlWindow : public Nxna::GraphicsDeviceManager
	{
		void* m_window;
		void* m_hdc;

	public:
		WindowsOpenGlWindow(Nxna::Game* game);
		virtual Nxna::Graphics::GraphicsDevice* CreateDevice() override;
		virtual void BeginDraw() override;
		virtual void EndDraw() override;
		virtual void ApplyChanges() override;
		virtual void ShowWindow() override;
		virtual void DestroyWindow() override;

		virtual void SetScreenSize(int width, int height, bool fullscreen) override;
	};

}
}
}

#endif // NXNA_PLATFORM_WINDOWS_WINDOWSOPENGLWINDOW_H