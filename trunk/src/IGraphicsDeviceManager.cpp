#include "IGraphicsDeviceManager.h"
#include "Game.h"

namespace Nxna
{
	GraphicsDeviceManager::GraphicsDeviceManager(Game* game)
	{
		game->m_graphicsDeviceManager = this;
	}
}