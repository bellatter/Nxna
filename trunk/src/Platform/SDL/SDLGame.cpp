#include "SDLGame.h"
#if defined NXNA_PLATFORM_APPLE
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif
#include "../../Audio/AudioManager.h"
#include "../../Utils/StopWatch.h"

namespace Nxna
{
namespace Platform
{
namespace SDL
{
	SDLGame::SDLGame(Game* game)
	{
		m_game = game;
		m_active = true;
		m_quitReceived = false;
		m_targetElapsedTime = 1.0f / 60.0f;
		m_isFixedTimeStep = true;

		SDL_Init(SDL_INIT_VIDEO);
	}

	void SDLGame::Init(int /* argc */, const char** /* argv */)
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

	void SDLGame::Run()
	{
		double accumulatedElapsedTime = 0;

		float timeAtLastDraw = 0;
		double targetElapsedTime = m_targetElapsedTime * 1000.0f;

		double freq = 1000.0 / (double)SDL_GetPerformanceFrequency();
		uint64_t prevTimeNow = SDL_GetPerformanceCounter();

		while(m_quitReceived == false)
		{
			uint64_t timeNow = SDL_GetPerformanceCounter();
			accumulatedElapsedTime += (timeNow - prevTimeNow) * freq;
			prevTimeNow = timeNow;
		
			bool needToDraw = false;

			if (m_isFixedTimeStep)
			{
				if (accumulatedElapsedTime < targetElapsedTime)
				{
					unsigned int timeRemainder = (unsigned int)(targetElapsedTime - accumulatedElapsedTime);
					if (timeRemainder > 0)
						SDL_Delay(timeRemainder);

					continue;
				}

				m_gameTime.ElapsedGameTime = m_targetElapsedTime;

				handleEvents();
				updateTime();

				Media::MediaPlayer::Tick();

				while (accumulatedElapsedTime >= targetElapsedTime)
				{
					accumulatedElapsedTime -= targetElapsedTime;

					m_gameTime.TotalGameTime += m_targetElapsedTime;
					m_game->Update(m_gameTime);

					needToDraw = true;
				}
			}
			else
			{
				handleEvents();
				updateTime();

				Media::MediaPlayer::Tick();

				float elapsedtime = MathHelper::Min(0.1f, m_gameTime.ElapsedGameTime);

				m_gameTime.TotalGameTime += elapsedtime;
				m_gameTime.ElapsedGameTime = elapsedtime;

				m_game->Update(m_gameTime);

				needToDraw = true;
			}

			if (needToDraw)
			{
				GameTime time;
				time.TotalGameTime = m_gameTime.TotalGameTime;
				time.ElapsedGameTime = m_realTime.TotalGameTime - timeAtLastDraw;

				m_game->m_graphicsDeviceManager->BeginDraw();
				m_game->Draw(time);
				m_game->m_graphicsDeviceManager->EndDraw();

				timeAtLastDraw = m_realTime.TotalGameTime;
			}
		}
	}

	void SDLGame::Exit()
	{
		m_quitReceived = true;
	}

	void SDLGame::IsActive(bool active)
	{
		m_active = active;
		
		if (active)
			m_game->OnActivated();
		else
			m_game->OnDeactivated();
	}

	Nxna::Input::Keys convertSDLK(SDL_Keycode sdlk)
	{
		if (sdlk == SDLK_BACKSPACE)
			return Nxna::Input::Keys::Back;
		if (sdlk == SDLK_RETURN)
			return Nxna::Input::Keys::Enter;
		if (sdlk == SDLK_ESCAPE)
			return Nxna::Input::Keys::Escape;
		if (sdlk == SDLK_SPACE)
			return Nxna::Input::Keys::Space;
		if (sdlk == SDLK_BACKQUOTE)
			return Nxna::Input::Keys::OemTilde;
		if (sdlk == SDLK_LSHIFT)
			return Nxna::Input::Keys::LeftShift;
		if (sdlk == SDLK_RSHIFT)
			return Nxna::Input::Keys::RightShift;
		if (sdlk == SDLK_LCTRL)
			return Nxna::Input::Keys::LeftControl;
		if (sdlk == SDLK_RCTRL)
			return Nxna::Input::Keys::RightControl;
		if (sdlk == SDLK_UP)
			return Nxna::Input::Keys::Up;
		if (sdlk == SDLK_DOWN)
			return Nxna::Input::Keys::Down;
		if (sdlk == SDLK_RIGHT)
			return Nxna::Input::Keys::Right;
		if (sdlk == SDLK_LEFT)
			return Nxna::Input::Keys::Left;
		if (sdlk == SDLK_PERIOD)
			return Nxna::Input::Keys::OemPeriod;

		if (sdlk >= SDLK_a && sdlk <= SDLK_z)
			return (Nxna::Input::Keys)((int)Nxna::Input::Keys::A + (sdlk - SDLK_a));
		if (sdlk >= SDLK_0 && sdlk <= SDLK_9)
			return (Nxna::Input::Keys)((int)Nxna::Input::Keys::D0 + (sdlk - SDLK_0));

		return Nxna::Input::Keys::None;
	}

	void SDLGame::handleEvents()
	{
		Nxna::Input::Touch::TouchPanel::Refresh();

		SDL_Event e;

		while(SDL_PollEvent(&e))
		{
			switch(e.type)
			{
			case SDL_QUIT:
				m_quitReceived = true;
				break;
			case SDL_MOUSEMOTION:
				Nxna::Input::Mouse::InjectMouseMove(e.motion.x, e.motion.y);
				break;
			case SDL_MOUSEBUTTONDOWN:
				Nxna::Input::Mouse::InjectMouseButton(e.button.button, true);
				break;
			case SDL_MOUSEBUTTONUP:
				Nxna::Input::Mouse::InjectMouseButton(e.button.button, false);
				break;
			case SDL_KEYDOWN:
				Nxna::Input::Keyboard::InjectKeyDown(convertSDLK(e.key.keysym.sym));
				break;
			case SDL_KEYUP:
				Nxna::Input::Keyboard::InjectKeyUp(convertSDLK(e.key.keysym.sym));
				break;
			}
		}
	}

	void SDLGame::updateTime()
	{
		unsigned int time = SDL_GetTicks();
		float currentTime = time / 1000.0f;

		m_realTime.ElapsedGameTime = currentTime - m_realTime.TotalGameTime;
		m_realTime.TotalGameTime = currentTime;
	}
}
}
}
