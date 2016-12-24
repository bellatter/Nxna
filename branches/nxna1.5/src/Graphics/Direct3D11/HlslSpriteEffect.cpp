#include "../../NxnaConfig.h"

#if !defined NXNA_DISABLE_D3D11

#include "HlslSpriteEffect.h"
#include "D3D11ConstantBuffer.h"
#include "d3d11.h"


#include "ShaderSource/Compiled/SpriteEffect_SpriteVertexShader.inc"
#include "ShaderSource/Compiled/SpriteEffect_SpritePixelShader.inc"

namespace Nxna
{
namespace Graphics
{
namespace Direct3D11
{
	HlslSpriteEffect::HlslSpriteEffect(Direct3D11Device* device, HlslEffect* hlslEffect)
		: Pvt::SpriteEffectPimpl(hlslEffect), m_hlslEffect(hlslEffect)
	{
		hlslEffect->CreateProgram("default", false, SpriteEffect_SpriteVertexShader, sizeof(SpriteEffect_SpriteVertexShader),
			SpriteEffect_SpritePixelShader, sizeof(SpriteEffect_SpritePixelShader));

		// create the cbuffer
		hlslEffect->AddConstantBuffer(true, false, 16 * sizeof(float), 1);

		// create the parameters
		hlslEffect->AddParameter("ModelViewProjection", EffectParameterType::Single, 16, 0, 0, 0);
		hlslEffect->AddParameter("Diffuse", EffectParameterType::Texture2D, 1, 0, 0, 0);
	}
}
}
}

#endif


