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
	HlslSpriteEffect::HlslSpriteEffect(Direct3D11Device* device)
		: HlslEffect(device)
	{
		AddPermutation(SpriteEffect_SpriteVertexShader, sizeof(SpriteEffect_SpriteVertexShader),
			SpriteEffect_SpritePixelShader, sizeof(SpriteEffect_SpritePixelShader));

		// create the parameters
		EffectParameter* modelViewProjection = new EffectParameter(this, EffectParameterType::Single, 16, 0, "ModelViewProjection");
		AddParameter(modelViewProjection);

		EffectParameter* diffuse = new EffectParameter(this, EffectParameterType::Texture2D, 1, 0, "Diffuse");
		AddParameter(diffuse);

		int indices[] = {0, 1};
		int offsets[] = {0, 0};
		ConstantBuffer cbuffer(new D3D11ConstantBuffer(device, true, 16 * sizeof(float), indices, offsets, 1));
		//ConstantBuffer cbuffer(new D3D11ConstantBuffer(device, true, 16 * sizeof(float), indices, offsets, 1));
		m_cbuffers.push_back(cbuffer);
	}
}
}
}

#endif


