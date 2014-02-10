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

		hlslEffect->CreateProgram("ColorEnabled", true, DualTextureEffect_DualTextureVS, sizeof(DualTextureEffect_DualTextureVS),
			DualTextureEffect_DualTexturePS, sizeof(DualTextureEffect_DualTexturePS));
		hlslEffect->CreateProgram("NoColor", true, DualTextureEffect_DualTextureNoColorVS, sizeof(DualTextureEffect_DualTextureNoColorVS),
			DualTextureEffect_DualTextureNoColorPS, sizeof(DualTextureEffect_DualTextureNoColorPS));

		hlslEffect->CreateProgram("default", false, nullptr, 0, nullptr, 0);

		// create the cbuffer
		hlslEffect->AddConstantBuffer(true, false, 16 * sizeof(float), 1);

		// create the parameters
		hlslEffect->AddParameter("ModelViewProjection", EffectParameterType::Single, 16, 0, 0, 0);
		hlslEffect->AddParameter("Diffuse", EffectParameterType::Texture2D, 1, 0, 0, 0);
		hlslEffect->AddParameter("Diffuse2", EffectParameterType::Texture2D, 1, 0, 0, 0);
	}

	void HlslDualTextureEffect::Apply(int programIndex)
	{
		Matrix worldView;
		Matrix worldViewProjection;

		Matrix::Multiply(m_world, m_view, worldView);
		Matrix::Multiply(worldView, m_projection, worldViewProjection);

		m_hlslEffect->GetParameter("ModelViewProjection")->SetValue(worldViewProjection.C);

		m_device->SetCurrentEffect(m_hlslEffect, programIndex);
	}
}
}
}

#endif


