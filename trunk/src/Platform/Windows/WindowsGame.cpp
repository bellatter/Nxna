#include "../../NxnaConfig.h"

#if defined NXNA_PLATFORM_WIN32

#include "WindowsGame.h"
#include "../../Audio/AudioManager.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace Nxna
{
namespace Platform
{
namespace Windows
{
	LARGE_INTEGER timerFrequency;
	LARGE_INTEGER timeAtStart;

	WindowsGame::WindowsGame(Game* game)
	{
		m_game = game;
		m_active = true;
		m_quitReceived = false;
		m_targetElapsedTime = 1.0f / 60.0f;
		m_isFixedTimeStep = true;

		QueryPerformanceCounter(&timeAtStart);
	}

	void WindowsGame::Init(int /* argc */, char** /* argv */)
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

		QueryPerformanceFrequency(&timerFrequency);
	}

	void WindowsGame::Run()
	{
		float timeAtLastUpdate = 0;
		float timeAtLastDraw = 0;

		while(m_quitReceived == false)
		{
			// Handle the windows messages.
			MSG msg;
			if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			handleEvents();
			updateTime();

			Media::MediaPlayer::Tick();
		
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

					m_game->Update(m_gameTime);

					timeAtLastUpdate = m_realTime.TotalGameTime;
					needToDraw = true;
				}
			}
			else
			{
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

	void WindowsGame::Exit()
	{
		m_quitReceived = true;
	}

	void WindowsGame::IsActive(bool active)
	{
		m_active = active;
		
		if (active)
			m_game->OnActivated();
		else
			m_game->OnDeactivated();
	}

	/*Nxna::Input::Keys convertSDLK(SDL_Keycode sdlk)
	{
		if (sdlk == SDLK_BACKSPACE)
			return Nxna::Input::Keys_Back;
		if (sdlk == SDLK_RETURN)
			return Nxna::Input::Keys_Enter;
		if (sdlk == SDLK_BACKQUOTE)
			return Nxna::Input::Keys_OemTilde;

		return Nxna::Input::Keys_None;
	}*/

	NXNA_ENUM(WinEventType)
		Quit,
		MouseMotion,
		MouseButtonDown,
		MouseButtonUp,
		KeyDown,
		KeyUp
	END_NXNA_ENUM(WinEventType)
	
	struct WinEvent
	{
		WinEventType Type;
		int ParamA, ParamB, ParamC;
	};
	
	const int MAX_EVENTS = 128;
	WinEvent eventQueue[MAX_EVENTS];
	int numEvents = 0;
	int firstEvent = 0;

	void addEvent(WinEvent e)
	{
		if (numEvents < MAX_EVENTS)
		{
			int index = (firstEvent + numEvents) % MAX_EVENTS;
			eventQueue[index] = e;

			numEvents++;
		}
	}

	bool popEvent(WinEvent& e)
	{
		if (numEvents > 0)
		{
			e = eventQueue[firstEvent++];
			if (firstEvent >= MAX_EVENTS)
				firstEvent = 0;

			numEvents--;

			return true;
		}

		return false;
	}

	void WindowsGame::handleEvents()
	{
		Nxna::Input::Touch::TouchPanel::Refresh();

		WinEvent e;

		while(popEvent(e))
		{
			switch(e.Type)
			{
			case WinEventType::Quit:
				m_quitReceived = true;
				break;
			case WinEventType::MouseMotion:
				Nxna::Input::Mouse::InjectMouseMove(e.ParamA, e.ParamB);
				break;
			case WinEventType::MouseButtonDown:
				Nxna::Input::Mouse::InjectMouseButton(e.ParamA, true);
				break;
			case WinEventType::MouseButtonUp:
				Nxna::Input::Mouse::InjectMouseButton(e.ParamA, false);
				break;
			case WinEventType::KeyDown:
				Nxna::Input::Keyboard::InjectKeyDown(translateVirtualKey(e.ParamA));
				break;
			case WinEventType::KeyUp:
				Nxna::Input::Keyboard::InjectKeyUp(translateVirtualKey(e.ParamA));
				break;
			}
		}
	}

	void WindowsGame::updateTime()
	{
		LARGE_INTEGER currentTime;
		QueryPerformanceCounter(&currentTime);

		double time = (double)(currentTime.QuadPart - timeAtStart.QuadPart) / (double)timerFrequency.QuadPart;

		m_realTime.ElapsedGameTime = (float)(time - m_preciseRealTime);
		m_realTime.TotalGameTime = (float)time;
		m_preciseRealTime = time;
	}

	Nxna::Input::Keys WindowsGame::translateVirtualKey(int key)
	{
		// TODO: add the rest of the keys
		if (key == VK_RETURN)
			return Nxna::Input::Keys::Enter;
		if (key == VK_LEFT)
			return Nxna::Input::Keys::Left;
		if (key == VK_RIGHT)
			return Nxna::Input::Keys::Right;
		if (key == VK_UP)
			return Nxna::Input::Keys::Up;
		if (key == VK_DOWN)
			return Nxna::Input::Keys::Down;
		if (key == VK_OEM_3)
			return Nxna::Input::Keys::OemTilde;

		return Nxna::Input::Keys::None;
	}
}
}
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch(umessage)
	{
		// Check if the window is being closed.
		case WM_CLOSE:
		{
			Nxna::Platform::Windows::WinEvent e;
			e.Type = Nxna::Platform::Windows::WinEventType::Quit;
			Nxna::Platform::Windows::addEvent(e);

			return 0;
		}
		case WM_MOUSEMOVE:
		{
			Nxna::Platform::Windows::WinEvent e;
			e.Type = Nxna::Platform::Windows::WinEventType::MouseMotion;
			e.ParamA = LOWORD(lparam);
			e.ParamB = HIWORD(lparam);
			Nxna::Platform::Windows::addEvent(e);

			return 0;
		}
		case WM_LBUTTONDOWN:
		{
			Nxna::Platform::Windows::WinEvent e;
			e.Type = Nxna::Platform::Windows::WinEventType::MouseButtonDown;
			e.ParamA = 1;
			e.ParamB = LOWORD(lparam);
			e.ParamC = HIWORD(lparam);
			Nxna::Platform::Windows::addEvent(e);

			return 0;
		}
		case WM_LBUTTONUP:
		{
			Nxna::Platform::Windows::WinEvent e;
			e.Type = Nxna::Platform::Windows::WinEventType::MouseButtonUp;
			e.ParamA = 1;
			e.ParamB = LOWORD(lparam);
			e.ParamC = HIWORD(lparam);
			Nxna::Platform::Windows::addEvent(e);

			return 0;
		}
		case WM_KEYDOWN:
		{
			Nxna::Platform::Windows::WinEvent e;
			e.Type = Nxna::Platform::Windows::WinEventType::KeyDown;
			e.ParamA = wparam;
			e.ParamB = lparam;
			e.ParamC = 0;
			Nxna::Platform::Windows::addEvent(e);

			return 0;
		}
		case WM_KEYUP:
		{
			Nxna::Platform::Windows::WinEvent e;
			e.Type = Nxna::Platform::Windows::WinEventType::KeyUp;
			e.ParamA = wparam;
			e.ParamB = lparam;
			e.ParamC = 0;
			Nxna::Platform::Windows::addEvent(e);

			return 0;
		}
	}

	return DefWindowProc( hwnd, umessage, wparam, lparam );
}

#endif
