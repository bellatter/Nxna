#include "IOSGame.h"
#include "../../Nxna.h"

namespace Nxna
{
namespace Platform
{
namespace iOS
{
	IOSGame* g_instance = nullptr;

	IOSGame::IOSGame(Game* game)
	{
		m_game = game;
		m_currentTime = 0;

		g_instance = this;
	}

	void IOSGame::InitMeFirst()
	{
		// TODO: start up the app. This will begin the game loop.
	}

	void IOSGame::Init()
	{
		if (m_game->m_graphicsDeviceManager != nullptr)
		{
			m_game->m_graphicsDeviceManager->PreferredBackBufferWidth(800);
			m_game->m_graphicsDeviceManager->PreferredBackBufferHeight(480);
			m_game->m_device = m_game->m_graphicsDeviceManager->CreateDevice();
		}

		Audio::AudioManager::Init();

		m_game->Initialize();

		if (m_game->m_graphicsDeviceManager != nullptr)
			m_game->m_graphicsDeviceManager->ShowWindow();
	}

	void IOSGame::Update(float elapsedTime)
	{
		m_currentTime += elapsedTime;

		GameTime time;
		time.TotalGameTime = m_currentTime;
		time.ElapsedGameTime = elapsedTime;

		//m_game->Update(time);
	}

	void IOSGame::Draw(float elapsedTime)
	{
		GameTime time;
		time.TotalGameTime = m_currentTime;
		time.ElapsedGameTime = elapsedTime;

		//m_game->Draw(time);
	}
}
}
}

void IOSGame_Init()
{
	Nxna::Platform::iOS::g_instance->Init();
}

void IOSGame_Update(float elapsedTime)
{
	Nxna::Platform::iOS::g_instance->Update(elapsedTime);
}

void IOSGame_Draw(float elapsedTime)
{
	Nxna::Platform::iOS::g_instance->Draw(elapsedTime);
}

void IOSGame_TouchDown(float x, float y)
{
	Nxna::Input::Mouse::InjectMouseButton(0, true);
}

void IOSGame_TouchUp(float x, float y)
{
	Nxna::Input::Mouse::InjectMouseButton(0, false);
}

void IOSGame_TouchMoved(float x, float y)
{
	Nxna::Input::Mouse::InjectMouseMove((int)x, (int)y);
}
