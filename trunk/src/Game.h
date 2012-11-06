#ifndef NXNA_GAME_H
#define NXNA_GAME_H

#include "GameTime.h"
#include "Platform/PlatformDefs.h"

namespace Nxna
{
	namespace Content
	{
		class ContentManager;
	}

	namespace Graphics
	{
		class GraphicsDevice;
	}

	class IGraphicsDeviceManager;
	class GraphicsDeviceManager;

	namespace Platform
	{
	namespace iOS
	{
		class IOSGame;
	}
	namespace Windows
	{
		class WindowsGame;
	}
	namespace SDL
	{
		class SDLGame;
	}
	}

	class Game
	{
		friend class GraphicsDeviceManager;
		friend class Platform::iOS::IOSGame;
		friend class Platform::Windows::WindowsGame;
		friend class Platform::SDL::SDLGame;

#if defined NXNA_PLATFORM_APPLE_IOS
		Platform::iOS::IOSGame* m_pimpl;
#elif defined NXNA_PLATFORM_WIN32
		Platform::Windows::WindowsGame* m_pimpl;
		//Platform::SDL::SDLGame* m_pimpl;
#else
		Platform::SDL::SDLGame* m_pimpl;
#endif

		float m_targetElapsedTime;
		bool m_exitRequested;
		Nxna::Content::ContentManager* m_content;
		Nxna::Graphics::GraphicsDevice* m_device;
		GraphicsDeviceManager* m_graphicsDeviceManager;
		GameTime m_gameTime;
		GameTime m_realTime;
		bool m_isFixedTimeStep;

	public:
		Game();
		virtual ~Game();

		bool IsFixedTimeStep() { return m_isFixedTimeStep; }
		void IsFixedTimeStep(bool isFixedTimeStep) { m_isFixedTimeStep = isFixedTimeStep; }

		Nxna::Content::ContentManager* GetContent() { return m_content; }
		Nxna::Graphics::GraphicsDevice* GetGraphicsDevice() { return m_device; }
		
		// TODO: This doesn't exist in XNA. This should be GetWindow().
		GraphicsDeviceManager* GetGraphicsDeviceManager() { return m_graphicsDeviceManager; }

		float GetTargetElapsedTime() { return m_targetElapsedTime; }
		void SetTargetElapsedTime(float targetElapsedTime) { m_targetElapsedTime = targetElapsedTime; }

		void Run();
		void Run(int argc, char** argv);
		void Exit();
		
		bool IsActive();

	protected:

		virtual void Initialize()
		{
			// XNA seems to call LoadContent() within Initialize()
			LoadContent(); 
		}

		virtual void LoadContent() { }
		virtual void Update(const Nxna::GameTime& time) { }
		virtual void Draw(const Nxna::GameTime& time) { }
		virtual void UnloadContent() { }
		virtual void OnExiting() { }
		
		virtual void OnActivated() { }
		virtual void OnDeactivated() { }
	};
}

#endif // NXNA_GAME_H