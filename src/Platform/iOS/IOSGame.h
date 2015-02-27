#ifndef NXNA_PLATFORM_IOS_IOSGAME_H
#define NXNA_PLATFORM_IOS_IOSGAME_H

#include "../../Nxna.h"

namespace Nxna
{
namespace Platform
{
namespace iOS
{
	class IOSGame
	{
		float m_lastElapsedTime;
		float m_currentTime;
		bool m_active;
		Game* m_game;

	public:
		IOSGame(Game* game);

		void InitMeFirst(int argc, const char** argv);
		void Init();
		void Update(float elapsedTime);
		void Draw(float elapsedTime);
		void Exit();
		
		bool IsActive() { return m_active; }
		void IsActive(bool active);
	};
}
}
}

#endif // NXNA_PLATFORM_IOS_IOSGAME_H
