#ifndef IGRAPHICSDEVICEMANAGER_H
#define IGRAPHICSDEVICEMANAGER_H

namespace Nxna
{
	namespace Graphics
	{
		class GraphicsDevice;
	}

	class IGraphicsDeviceManager
	{
	public:
		virtual void BeginDraw() = 0;
		virtual void EndDraw() = 0;
		virtual Nxna::Graphics::GraphicsDevice* CreateDevice() = 0;
	};

	class Game;

	class GraphicsDeviceManager : public IGraphicsDeviceManager
	{
		int m_backBufferWidth;
		int m_backBufferHeight;
		bool m_fullscreen;

	protected:
		Graphics::GraphicsDevice* m_device;

	public:

		GraphicsDeviceManager(Game* game);

		Graphics::GraphicsDevice* GetGraphicsDevice() { return m_device; }

		int PreferredBackBufferWidth() { return m_backBufferWidth; }
		void PreferredBackBufferWidth(int width) { m_backBufferWidth = width; }
		int PreferredBackBufferHeight() { return m_backBufferHeight; }
		void PreferredBackBufferHeight(int height) { m_backBufferHeight = height; }
		bool IsFullscreen() { return m_fullscreen; }
		void IsFullscreen(bool fullscreen) { m_fullscreen = fullscreen; }

		virtual void ApplyChanges() = 0;

		virtual void ShowWindow() = 0;
		virtual void DestroyWindow() = 0;

		// TODO: don't bother with any of the other methods, like ApplyChanges().
		// Those don't work properly yet. This is the only method currently guaranteed
		// (more or less) to work, though you can only call it ONCE.
		virtual void SetScreenSize(int width, int height, bool fullscreen) = 0;
	};
}

#endif // IGRAPHICSDEVICEMANAGER_H
