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
	HlslBasicEffect::HlslBasicEffect(Direct3D11Device* device, HlslEffect* hlslEffect)
		: Pvt::BasicEffectPimpl(hlslEffect), m_hlslEffect(hlslEffect)
	{
		m_device = device;

		hlslEffect->CreateProgram("ColorTexture", true, BasicEffect_BasicEffectColorTextureVS, sizeof(BasicEffect_BasicEffectColorTextureVS),
			BasicEffect_BasicEffectColorTexturePS, sizeof(BasicEffect_BasicEffectColorTexturePS));
		hlslEffect->CreateProgram("Color", true, BasicEffect_BasicEffectColorVS, sizeof(BasicEffect_BasicEffectColorVS),
			BasicEffect_BasicEffectColorPS, sizeof(BasicEffect_BasicEffectColorPS));
		hlslEffect->CreateProgram("Texture", true, BasicEffect_BasicEffectTextureVS, sizeof(BasicEffect_BasicEffectTextureVS),
			BasicEffect_BasicEffectTexturePS, sizeof(BasicEffect_BasicEffectTexturePS));
		hlslEffect->CreateProgram("Other", true, BasicEffect_BasicEffectVS, sizeof(BasicEffect_BasicEffectVS),
			BasicEffect_BasicEffectPS, sizeof(BasicEffect_BasicEffectPS));

		// create the non-hidden dummy technique
		hlslEffect->CreateProgram("default", false, nullptr, 0, nullptr, 0);

		// create the cbuffer
		hlslEffect->AddConstantBuffer(true, false, 16 * sizeof(float), 1);

		// create the parameters
		hlslEffect->AddParameter("ModelViewProjection", EffectParameterType::Single, 16, 0, 0, 0);
		hlslEffect->AddParameter("Diffuse", EffectParameterType::Texture2D, 1, 0, 0, 0);
	}

	void HlslBasicEffect::Apply(int programIndex)
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


