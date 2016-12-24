#include "SpriteEffect.h"
#include "GraphicsDevice.h"

#ifdef NXNA_PLATFORM_WIN32
#include "Effects/SpriteEffect.inc"
#else
#include "Effects/SpriteEffect_nohlsl.inc"
#endif

namespace Nxna
{
namespace Graphics
{
	SpriteEffect::SpriteEffect(GraphicsDevice* device)
		: Effect(device, (byte*)SpriteEffect_bytecode, sizeof(SpriteEffect_bytecode))
	{
	}
}
}