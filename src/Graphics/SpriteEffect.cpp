#include "SpriteEffect.h"
#include "GraphicsDevice.h"

#include "Effects/SpriteEffect.inc"

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