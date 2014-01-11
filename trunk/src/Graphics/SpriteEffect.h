#ifndef GRAPHICS_SPRITEEFFECT_H
#define GRAPHICS_SPRITEEFFECT_H

#include "Effect.h"

namespace Nxna
{
namespace Graphics
{
	namespace Pvt
	{
		class SpriteEffectPimpl;
	}

	class SpriteEffect : public Effect
	{
		Pvt::SpriteEffectPimpl* m_sePimpl;

	public:

		SpriteEffect(GraphicsDevice* device);
		virtual ~SpriteEffect() {}
	};
}
}

#endif // GRAPHICS_SPRITEEFFECT_H
