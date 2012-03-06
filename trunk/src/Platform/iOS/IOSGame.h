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
		float m_currentTime;
		Game* m_game;

	public:
		IOSGame(Game* game);

		void InitMeFirst();
		void Init();
		void Update(float elapsedTime);
		void Draw(float elapsedTime);
	};
}
}
}

#endif // NXNA_PLATFORM_IOS_IOSGAME_H
