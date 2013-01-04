#include "IOSGame.h"
#include "IOSGame_c.h"
#include "../../Nxna.h"
#include "../../Audio/AudioManager.h"

extern "C"
{
int NxnaMainIOS(int argc, char *argv[]);
}

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
		m_lastElapsedTime = 0;
		m_active = true;

		g_instance = this;
	}

	void IOSGame::InitMeFirst(int argc, char** argv)
	{
		// start up the app. This will begin the game loop.
		NxnaMainIOS(argc, argv);
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
		m_lastElapsedTime = elapsedTime;
		m_currentTime += elapsedTime;

		GameTime time;
		time.TotalGameTime = m_currentTime;
		time.ElapsedGameTime = elapsedTime;

		Media::MediaPlayer::Tick();

		m_game->Update(time);
	}

	void IOSGame::Draw(float elapsedTime)
	{
		GameTime time;
		time.TotalGameTime = m_currentTime;
		time.ElapsedGameTime = elapsedTime;

		m_game->Draw(time);
	}
	
	void IOSGame::Exit()
	{
		// TODO
	}
	
	void IOSGame::IsActive(bool active)
	{
		m_active = active;
		
		if (active)
			m_game->OnActivated();
		else
			m_game->OnDeactivated();
		
		// force a call to update so the game has a chance to pause
		if (active == false)
		{
			m_currentTime += m_lastElapsedTime;
			
			GameTime time;
			time.TotalGameTime = m_currentTime;
			time.ElapsedGameTime = m_lastElapsedTime;
			
			m_game->Update(time);
		}
	}
}
}
}

extern "C"
{

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

void IOSGame_IsActive(bool active)
{
	Nxna::Platform::iOS::g_instance->IsActive(active);
}

void IOSGame_TouchDown(float x, float y)
{
	Nxna::Input::Mouse::InjectMouseMove((int)x, (int)y);
	Nxna::Input::Mouse::InjectMouseButton(1, true);
}

void IOSGame_TouchUp(float x, float y)
{
	Nxna::Input::Mouse::InjectMouseButton(1, false);
}

void IOSGame_TouchMoved(float x, float y)
{
	Nxna::Input::Mouse::InjectMouseMove((int)x, (int)y);
}
	
}
