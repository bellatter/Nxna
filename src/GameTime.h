#ifndef GAMETIME_H
#define GAMETIME_H

namespace Nxna
{
	struct GameTime
	{
		float ElapsedGameTime;
		float TotalGameTime;

		GameTime()
		{
			ElapsedGameTime = 0;
			TotalGameTime = 0;
		}
	};
}

#endif // GAMETIME_H