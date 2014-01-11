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
	HlslDualTextureEffect::HlslDualTextureEffect(Direct3D11Device* device, HlslEffect* hlslEffect)
		: Pvt::DualTextureEffectPimpl(hlslEffect), m_hlslEffect(hlslEffect)
	{
		m_device = device;

		m_isVertexColorEnabled = false;

		hlslEffect->AddPermutation(DualTextureEffect_DualTextureVS, sizeof(DualTextureEffect_DualTextureVS),
			DualTextureEffect_DualTexturePS, sizeof(DualTextureEffect_DualTexturePS));
		hlslEffect->AddPermutation(DualTextureEffect_DualTextureNoColorVS, sizeof(DualTextureEffect_DualTextureNoColorVS),
			DualTextureEffect_DualTextureNoColorPS, sizeof(DualTextureEffect_DualTextureNoColorPS));

		// create the parameters
		hlslEffect->AddParameter(EffectParameterType::Single, 16, 0, "ModelViewProjection");
		hlslEffect->AddParameter(EffectParameterType::Texture2D, 1, 0, "Diffuse");
		hlslEffect->AddParameter(EffectParameterType::Texture2D, 1, 0, "Diffuse2");

		int indices[] = {0, 1};
		int offsets[] = {0, 0};
		ConstantBuffer cbuffer(new D3D11ConstantBuffer(device, true, false, 16 * sizeof(float), indices, offsets, 1));
		//ConstantBuffer cbuffer(new D3D11ConstantBuffer(device, true, 16 * sizeof(float), indices, offsets, 1));
		hlslEffect->GetConstantBuffers().push_back(cbuffer);
	}

	void HlslDualTextureEffect::Apply()
	{
		Matrix worldView;
		Matrix worldViewProjection;

		Matrix::Multiply(m_world, m_view, worldView);
		Matrix::Multiply(worldView, m_projection, worldViewProjection);

		m_hlslEffect->GetParameter("ModelViewProjection")->SetValue(worldViewProjection.C);

		if (m_isVertexColorEnabled)
			m_device->SetCurrentEffect(m_hlslEffect, 0);
		else
			m_device->SetCurrentEffect(m_hlslEffect, 1);
	}
}
}
}

#endif


