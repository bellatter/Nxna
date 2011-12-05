#ifndef NXNA_PLATFORM_OPENGLWINDOW_H
#define NXNA_PLATFORM_OPENGLWINDOW_H

#include "../IGraphicsDeviceManager.h"

namespace Nxna
{
namespace Platform
{
	class OpenGlWindow : public Nxna::GraphicsDeviceManager
	{
		void* m_window;

	public:
		OpenGlWindow(Nxna::Game* game);
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

#endif // NXNA_PLATFORM_OPENGLWINDOW_H
