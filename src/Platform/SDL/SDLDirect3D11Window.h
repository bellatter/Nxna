#ifndef NXNA_PLATFORM_SDL_SDLDIRECT3D11WINDOW_H
#define NXNA_PLATFORM_SDL_SDLDIRECT3D11WINDOW_H

#include "../../NxnaConfig.h"
#include "../../IGraphicsDeviceManager.h"

NXNA_DISABLE_OVERRIDE_WARNING

namespace Nxna
{
namespace Platform
{
namespace SDL
{
	class SDLDirect3D11Window : public Nxna::GraphicsDeviceManager
	{
		void* m_window;

	public:
		SDLDirect3D11Window(Nxna::Game* game);
		virtual Nxna::Graphics::GraphicsDevice* CreateDevice() override;
		virtual void BeginDraw() override;
		virtual void EndDraw() override;
		virtual void ApplyChanges() override;
		virtual void ShowWindow() override;
		virtual void DestroyWindow() override;

		virtual void SetScreenSize(const Graphics::PresentationParameters& pp) override;
	};
}
}
}

NXNA_ENABLE_OVERRIDE_WARNING

#endif // NXNA_PLATFORM_SDL_SDLDIRECT3D11WINDDOW_H