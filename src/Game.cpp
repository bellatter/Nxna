#include "Game.h"
#include "Content/ContentManager.h"
#include "IGraphicsDeviceManager.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/SpriteBatch.h"
#include "Audio/AudioManager.h"
#include "MathHelper.h"

#if defined NXNA_PLATFORM_APPLE_IOS
#include "Platform/iOS/IOSGame.h"
#elif defined NXNA_PLATFORM_WIN32
#include "Platform/Windows/WindowsGame.h"
#include "Platform/SDL/SDLGame.h"
#else
#include "Platform/SDL/SDLGame.h"
#endif

namespace Nxna
{
	Game::Game()
	{
		m_exitRequested = false;
		m_targetElapsedTime = 1.0f / 60.0f;
		m_isFixedTimeStep = true;

		m_graphicsDeviceManager = nullptr;
		m_device = nullptr;
		m_content = new Nxna::Content::ContentManager();

#if defined NXNA_PLATFORMENGINE_IOS
		m_pimpl = new Platform::iOS::IOSGame(this);
#else
		m_pimpl = new Platform::SDL::SDLGame(this);
#endif
	}

	Game::~Game()
	{
		delete m_content;
		delete m_pimpl;
	}

	void Game::Run()
	{
		char* argv[] = {"\0"};
		Run(0, argv);
	}
	
	void Game::Run(int argc, char** argv)
	{

#if defined NXNA_PLATFORM_APPLE_IOS
		m_pimpl->InitMeFirst(argc, argv);
#else
		m_pimpl->Init(argc, argv);
		m_pimpl->Run();
#endif

		OnExiting();

		UnloadContent();
		m_content->Unload(); // maybe the user already did this, but we'll make sure.

		Graphics::SpriteBatch::Internal_Shutdown();

		Audio::AudioManager::Shutdown();
		delete m_device;

		if (m_graphicsDeviceManager != nullptr)
			m_graphicsDeviceManager->DestroyWindow();

#ifdef NXNA_PLATFORM_APPLE_IOS
		exit(0);
#endif
	}

	void Game::Exit()
	{
		m_pimpl->Exit();
	}
	
	bool Game::IsActive()
	{
		return m_pimpl->IsActive();
	}
}