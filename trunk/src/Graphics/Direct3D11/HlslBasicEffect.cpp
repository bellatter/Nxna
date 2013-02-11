#include "../../NxnaConfig.h"

#if !defined NXNA_DISABLE_D3D11

#include "HlslBasicEffect.h"
#include "Direct3D11Device.h"
#include "D3D11ConstantBuffer.h"
#include "d3d11.h"


#include "ShaderSource/Compiled/BasicEffect_BasicEffectVS.inc"
#include "ShaderSource/Compiled/BasicEffect_BasicEffectColorVS.inc"
#include "ShaderSource/Compiled/BasicEffect_BasicEffectTextureVS.inc"
#include "ShaderSource/Compiled/BasicEffect_BasicEffectColorTextureVS.inc"
#include "ShaderSource/Compiled/BasicEffect_BasicEffectPS.inc"
#include "ShaderSource/Compiled/BasicEffect_BasicEffectColorPS.inc"
#include "ShaderSource/Compiled/BasicEffect_BasicEffectTexturePS.inc"
#include "ShaderSource/Compiled/BasicEffect_BasicEffectColorTexturePS.inc"

namespace Nxna
{
namespace Graphics
{
namespace Direct3D11
{
	HlslBasicEffect::HlslBasicEffect(Direct3D11Device* device)
		: HlslEffect(device)
	{
		m_finalTransformDirty = true;
		m_isTextureEnabled = false;
		m_isVertexColorEnabled = false;

		AddPermutation(BasicEffect_BasicEffectColorTextureVS, sizeof(BasicEffect_BasicEffectColorTextureVS),
			BasicEffect_BasicEffectColorTexturePS, sizeof(BasicEffect_BasicEffectColorTexturePS));
		AddPermutation(BasicEffect_BasicEffectColorVS, sizeof(BasicEffect_BasicEffectColorVS),
			BasicEffect_BasicEffectColorPS, sizeof(BasicEffect_BasicEffectColorPS));
		AddPermutation(BasicEffect_BasicEffectTextureVS, sizeof(BasicEffect_BasicEffectTextureVS),
			BasicEffect_BasicEffectTexturePS, sizeof(BasicEffect_BasicEffectTexturePS));
		AddPermutation(BasicEffect_BasicEffectVS, sizeof(BasicEffect_BasicEffectVS),
			BasicEffect_BasicEffectPS, sizeof(BasicEffect_BasicEffectPS));

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

	void HlslBasicEffect::SetTexture(Texture2D* texture)
	{
		GetParameter("Diffuse")->SetValue(texture);
	}

	void HlslBasicEffect::Apply()
	{
		Matrix worldView;
		Matrix worldViewProjection;

		Matrix::Multiply(m_world, m_view, worldView);
		Matrix::Multiply(worldView, m_projection, worldViewProjection);

		GetParameter("ModelViewProjection")->SetValue(worldViewProjection.C);

		if (m_isVertexColorEnabled && m_isTextureEnabled)
			static_cast<Direct3D11Device*>(m_device)->SetCurrentEffect(this, 0);
		else if (m_isVertexColorEnabled)
			static_cast<Direct3D11Device*>(m_device)->SetCurrentEffect(this, 1);
		else if (m_isTextureEnabled)
			static_cast<Direct3D11Device*>(m_device)->SetCurrentEffect(this, 2);
		else
			static_cast<Direct3D11Device*>(m_device)->SetCurrentEffect(this, 3);
	}
}
}
}

#endif


