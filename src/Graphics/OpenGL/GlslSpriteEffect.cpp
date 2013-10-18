#include "GlslSpriteEffect.h"

namespace Nxna
{
namespace Graphics
{
namespace OpenGl
{

#include "ShaderSource/SpriteEffect.vert.inc"
#include "ShaderSource/SpriteEffect.frag.inc"

	GlslSpriteEffect::GlslSpriteEffect(OpenGlDevice* device)
		: GlslEffect(device, SpriteEffect_vert, SpriteEffect_frag)
	{
	}
}
}
}

