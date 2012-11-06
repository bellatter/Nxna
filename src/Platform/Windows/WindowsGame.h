#ifndef NXNA_PLATFORM_WINDOWS_WINDOWSGAME_H
#define NXNA_PLATFORM_WINDOWS_WINDOWSGAME_H

#include "../../Nxna.h"

namespace Nxna
{
namespace Platform
{
namespace Windows
{
	class WindowsGame
	{
		bool m_active;
		bool m_quitReceived;
		GameTime m_gameTime;
		GameTime m_realTime;
		double m_preciseRealTime;
		bool m_isFixedTimeStep;
		float m_targetElapsedTime;
		Game* m_game;

	public:
		WindowsGame(Game* game);
		void Init();
		void Run();
		void Exit();

		bool IsActive() { return m_active; }
		void IsActive(bool active);

	private:
		void handleEvents();
		void updateTime();
	};
}
}
}

#endif // NXNA_PLATFORM_WINDOWS_WINDOWSGAME_H
