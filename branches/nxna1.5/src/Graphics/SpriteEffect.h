#ifndef GRAPHICS_SPRITEEFFECT_H
#define GRAPHICS_SPRITEEFFECT_H

#include "Effect.h"

namespace Nxna
{
namespace Graphics
{
	class SpriteEffect : public Effect
	{
	public:

		SpriteEffect(GraphicsDevice* device);
		virtual ~SpriteEffect() {}
	};
}
}

#endif // GRAPHICS_SPRITEEFFECT_H
