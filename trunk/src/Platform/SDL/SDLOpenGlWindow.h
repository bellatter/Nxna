#ifndef NXNA_PLATFORM_SDL_SDLOPENGLWINDOW_H
#define NXNA_PLATFORM_SDL_SDLOPENGLWINDOW_H

#include "../../NxnaConfig.h"
#include "../../IGraphicsDeviceManager.h"

namespace Nxna
{
namespace Platform
{
namespace SDL
{
	class SDLOpenGlWindow : public Nxna::GraphicsDeviceManager
	{
		void* m_window;
		void* m_glContext;

	public:
		SDLOpenGlWindow(Nxna::Game* game);
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

#endif // NXNA_PLATFORM_SDL_SDLOPENGLWINDOW_H
