#ifndef NXNA_PLATFORM_WINDOWS_WINDOWSDIRECT3D11WINDOW_H
#define NXNA_PLATFORM_WINDOWS_WINDOWSDIRECT3D11WINDOW_H

#include "../../NxnaConfig.h"
#include "../../IGraphicsDeviceManager.h"

namespace Nxna
{
namespace Platform
{
namespace Windows
{
	class WindowsDirect3D11Window : public Nxna::GraphicsDeviceManager
	{
		void* m_window;

	public:
		WindowsDirect3D11Window(Nxna::Game* game);
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

#endif // NXNA_PLATFORM_WINDOWS_WINDOWSDIRECT3D11WINDOW_H