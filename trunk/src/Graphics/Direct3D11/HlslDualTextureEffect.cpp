#include "../../NxnaConfig.h"

#if !defined NXNA_DISABLE_D3D11

#include "HlslDualTextureEffect.h"
#include "Direct3D11Device.h"
#include "D3D11ConstantBuffer.h"
#include "d3d11.h"


#include "ShaderSource/Compiled/DualTextureEffect_DualTextureVS.inc"
#include "ShaderSource/Compiled/DualTextureEffect_DualTexturePS.inc"
#include "ShaderSource/Compiled/DualTextureEffect_DualTextureNoColorVS.inc"
#include "ShaderSource/Compiled/DualTextureEffect_DualTextureNoColorPS.inc"

namespace Nxna
{
namespace Graphics
{
namespace Direct3D11
{
	HlslDualTextureEffect::HlslDualTextureEffect(Direct3D11Device* device)
		: HlslEffect(device)
	{
		m_isVertexColorEnabled = false;

		AddPermutation(DualTextureEffect_DualTextureVS, sizeof(DualTextureEffect_DualTextureVS),
			DualTextureEffect_DualTexturePS, sizeof(DualTextureEffect_DualTexturePS));
		AddPermutation(DualTextureEffect_DualTextureNoColorVS, sizeof(DualTextureEffect_DualTextureNoColorVS),
			DualTextureEffect_DualTextureNoColorPS, sizeof(DualTextureEffect_DualTextureNoColorPS));

		// create the parameters
		EffectParameter* modelViewProjection = new EffectParameter(this, EffectParameterType::Single, 16, 0, "ModelViewProjection");
		AddParameter(modelViewProjection);

		EffectParameter* diffuse = new EffectParameter(this, EffectParameterType::Texture2D, 1, 0, "Diffuse");
		AddParameter(diffuse);

		EffectParameter* diffuse2 = new EffectParameter(this, EffectParameterType::Texture2D, 1, 0, "Diffuse2");
		AddParameter(diffuse2);

		int indices[] = {0, 1};
		int offsets[] = {0, 0};
		ConstantBuffer cbuffer(new D3D11ConstantBuffer(device, true, 16 * sizeof(float), indices, offsets, 1));
		//ConstantBuffer cbuffer(new D3D11ConstantBuffer(device, true, 16 * sizeof(float), indices, offsets, 1));
		m_cbuffers.push_back(cbuffer);
	}

	void HlslDualTextureEffect::SetTexture(Texture2D* texture)
	{
		GetParameter("Diffuse")->SetValue(texture);
	}

	void HlslDualTextureEffect::SetTexture2(Texture2D* texture)
	{
		GetParameter("Diffuse2")->SetValue(texture);
	}

	void HlslDualTextureEffect::Apply()
	{
		Matrix worldView;
		Matrix worldViewProjection;

		Matrix::Multiply(m_world, m_view, worldView);
		Matrix::Multiply(worldView, m_projection, worldViewProjection);

		GetParameter("ModelViewProjection")->SetValue(worldViewProjection.C);

		if (m_isVertexColorEnabled)
			static_cast<Direct3D11Device*>(m_device)->SetCurrentEffect(this, 0);
		else
			static_cast<Direct3D11Device*>(m_device)->SetCurrentEffect(this, 1);
	}
}
}
}

#endif


