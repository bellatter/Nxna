#include "Game.h"
#include "Platform/Input.h"
#include "Content/ContentManager.h"
#include "IGraphicsDeviceManager.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/SpriteBatch.h"
#include "Audio/AudioManager.h"
#include "MathHelper.h"
#include "Platform/Clock.h"

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
	}

	Game::~Game()
	{
		delete m_content;
	}

	void Game::Run()
	{
		if (m_graphicsDeviceManager != nullptr)
		{
			m_graphicsDeviceManager->PreferredBackBufferWidth(800);
			m_graphicsDeviceManager->PreferredBackBufferHeight(480);
			m_device = m_graphicsDeviceManager->CreateDevice();
		}

		Audio::AudioManager::Init();

		Initialize();

		if (m_graphicsDeviceManager != nullptr)
			m_graphicsDeviceManager->ShowWindow();

		float timeAtLastUpdate = 0;
		float timeAtLastDraw = 0;

		while(m_exitRequested == false)
		{
			handleEvents();
			updateTime();
		
			bool needToDraw = false;

			if (m_isFixedTimeStep)
			{
				int timesToRun = (int)((m_realTime.TotalGameTime - timeAtLastUpdate) / m_targetElapsedTime);
				if (timesToRun > 20)
					timesToRun = 20;

				for (int i = 0; i < timesToRun; i++)
				{
					m_gameTime.TotalGameTime += m_targetElapsedTime;
					m_gameTime.ElapsedGameTime = m_targetElapsedTime;

					Update(m_gameTime);

					timeAtLastUpdate = m_realTime.TotalGameTime;
					needToDraw = true;
				}
			}
			else
			{
				float elapsedtime = MathHelper::Min(0.1f, m_gameTime.ElapsedGameTime);

				m_gameTime.TotalGameTime += elapsedtime;
				m_gameTime.ElapsedGameTime = elapsedtime;

				Update(m_gameTime);

				needToDraw = true;
			}

			if (needToDraw)
			{
				GameTime time;
				time.TotalGameTime = m_gameTime.TotalGameTime;
				time.ElapsedGameTime = m_realTime.TotalGameTime - timeAtLastDraw;

				m_graphicsDeviceManager->BeginDraw();
				Draw(time);
				m_graphicsDeviceManager->EndDraw();

				timeAtLastDraw = m_realTime.TotalGameTime;
			}
		}

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
		m_exitRequested = true;
	}

	void Game::handleEvents()
	{
		Nxna::Platform::Input::Refresh();

		if (Nxna::Platform::Input::WasQuitReceived())
			Exit();
	}

	void Game::updateTime()
	{
		unsigned int time = Platform::Clock::GetCurrentTime();
		float currentTime = time / 1000.0f;

		m_realTime.ElapsedGameTime = currentTime - m_realTime.TotalGameTime;
		m_realTime.TotalGameTime = currentTime;
	}
}