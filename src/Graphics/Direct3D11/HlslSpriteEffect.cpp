#include "HlslSpriteEffect.h"
#include "d3d11.h"

#include "ShaderSource/Compiled/SpriteEffect_SpriteVertexShader.inc"
#include "ShaderSource/Compiled/SpriteEffect_SpritePixelShader.inc"

namespace Nxna
{
namespace Graphics
{
namespace Direct3D11
{
	HlslSpriteEffect::HlslSpriteEffect(Direct3D11Device* device)
		: HlslEffect(device)
	{
		AddPermutation(SpriteEffect_SpriteVertexShader, sizeof(SpriteEffect_SpriteVertexShader), SpriteEffect_SpritePixelShader, sizeof(SpriteEffect_SpritePixelShader));
	}
}
}
}

