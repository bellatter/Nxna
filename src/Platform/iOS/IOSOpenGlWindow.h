#ifndef NXNA_PLATFORM_IOS_IOSOPENGLWINDOW_H
#define NXNA_PLATFORM_IOS_IOSOPENGLWINDOW_H

#include "../../NxnaConfig.h"
#include "../../IGraphicsDeviceManager.h"

namespace Nxna
{
namespace Platform
{
namespace iOS
{
	class IOSOpenGlWindow : public Nxna::GraphicsDeviceManager
	{
	public:
		IOSOpenGlWindow(Nxna::Game* game);
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

#endif // NXNA_PLATFORM_IOS_IOSOPENGLWINDOW_H
