#include "../../NxnaConfig.h"

#if !defined NXNA_DISABLE_D3D11

#include "HlslAlphaTestEffect.h"
#include "Direct3D11Device.h"
#include "D3D11ConstantBuffer.h"
#include "d3d11.h"

#include "ShaderSource/Compiled/AlphaTestEffect_VSAlphaTest.inc"
#include "ShaderSource/Compiled/AlphaTestEffect_VSAlphaTestNoFog.inc"
#include "ShaderSource/Compiled/AlphaTestEffect_VSAlphaTestVC.inc"
#include "ShaderSource/Compiled/AlphaTestEffect_VSAlphaTestVcNoFog.inc"
#include "ShaderSource/Compiled/AlphaTestEffect_PSAlphaTestEqNe.inc"
#include "ShaderSource/Compiled/AlphaTestEffect_PSAlphaTestEqNeNoFog.inc"
#include "ShaderSource/Compiled/AlphaTestEffect_PSAlphaTestLtGt.inc"
#include "ShaderSource/Compiled/AlphaTestEffect_PSAlphaTestLtGtNoFog.inc"

namespace Nxna
{
namespace Graphics
{
namespace Direct3D11
{
	HlslAlphaTestEffect::HlslAlphaTestEffect(Direct3D11Device* device, HlslEffect* hlslEffect)
		: Pvt::AlphaTestEffectPimpl(hlslEffect), m_hlslEffect(hlslEffect)
	{
		m_device = device;

		hlslEffect->AddPermutation(AlphaTestEffect_VSAlphaTestVcNoFog, sizeof(AlphaTestEffect_VSAlphaTestVcNoFog),
			AlphaTestEffect_PSAlphaTestLtGtNoFog , sizeof(AlphaTestEffect_PSAlphaTestLtGtNoFog));
		hlslEffect->AddPermutation(AlphaTestEffect_VSAlphaTestVcNoFog, sizeof(AlphaTestEffect_VSAlphaTestVcNoFog),
			AlphaTestEffect_PSAlphaTestEqNeNoFog, sizeof(AlphaTestEffect_PSAlphaTestEqNeNoFog));
		hlslEffect->AddPermutation(AlphaTestEffect_VSAlphaTestNoFog, sizeof(AlphaTestEffect_VSAlphaTestNoFog),
			AlphaTestEffect_PSAlphaTestLtGtNoFog , sizeof(AlphaTestEffect_PSAlphaTestLtGtNoFog));
		hlslEffect->AddPermutation(AlphaTestEffect_VSAlphaTestNoFog, sizeof(AlphaTestEffect_VSAlphaTestNoFog),
			AlphaTestEffect_PSAlphaTestEqNeNoFog, sizeof(AlphaTestEffect_PSAlphaTestEqNeNoFog));

		hlslEffect->AddPermutation(AlphaTestEffect_VSAlphaTestVc, sizeof(AlphaTestEffect_VSAlphaTestVc),
			AlphaTestEffect_PSAlphaTestLtGt , sizeof(AlphaTestEffect_PSAlphaTestLtGt));
		hlslEffect->AddPermutation(AlphaTestEffect_VSAlphaTestVc, sizeof(AlphaTestEffect_VSAlphaTestVc),
			AlphaTestEffect_PSAlphaTestEqNe, sizeof(AlphaTestEffect_PSAlphaTestEqNe));
		hlslEffect->AddPermutation(AlphaTestEffect_VSAlphaTest, sizeof(AlphaTestEffect_VSAlphaTest),
			AlphaTestEffect_PSAlphaTestLtGt, sizeof(AlphaTestEffect_PSAlphaTestLtGt));
		hlslEffect->AddPermutation(AlphaTestEffect_VSAlphaTest, sizeof(AlphaTestEffect_VSAlphaTest),
			AlphaTestEffect_PSAlphaTestEqNe , sizeof(AlphaTestEffect_PSAlphaTestEqNe ));

		// create the parameters
		hlslEffect->AddParameter(EffectParameterType::Single, 16, 0, "ModelViewProjection");
		hlslEffect->AddParameter(EffectParameterType::Texture2D, 1, 0, "Diffuse");
		hlslEffect->AddParameter(EffectParameterType::Single, 4, 0, "AlphaTest");
		hlslEffect->AddParameter(EffectParameterType::Single, 4, 0, "DiffuseColor");

		int indices[] = {0, 1, 2, 3};
		int offsets[] = {0, 0, 80, 64};
		ConstantBuffer cbuffer(new D3D11ConstantBuffer(device, true, true, 32 * sizeof(float), indices, offsets, 4));
		hlslEffect->GetConstantBuffers().push_back(cbuffer);
	}

	void HlslAlphaTestEffect::Apply()
	{
		Matrix worldView;
		Matrix worldViewProjection;

		Matrix::Multiply(m_world, m_view, worldView);
		Matrix::Multiply(worldView, m_projection, worldViewProjection);

		m_hlslEffect->GetParameter("ModelViewProjection")->SetValue(worldViewProjection.C);
		m_hlslEffect->GetParameter("DiffuseColor")->SetValue(Vector4(1.0f, 1.0f, 1.0f, 1.0f));

		// calculate the AlphaTest parameter
		const float threshold = 0.5f / 255.0f;
		Vector4 alphaTest;
		if (m_compareFunction == CompareFunction::Less)
		{
			alphaTest.X = m_referenceAlpha - threshold;
			alphaTest.Z = 1.0f;
			alphaTest.W = -1.0f;
		}
		else if (m_compareFunction == CompareFunction::LessEqual)
		{
			alphaTest.X = m_referenceAlpha + threshold;
			alphaTest.Z = 1.0f;
			alphaTest.W = -1.0f;
		}
		else if (m_compareFunction == CompareFunction::GreaterEqual)
		{
			alphaTest.X = m_referenceAlpha - threshold;
			alphaTest.Z = -1.0f;
			alphaTest.W = 1.0f;
		}
		else if (m_compareFunction == CompareFunction::Greater)
		{
			alphaTest.X = m_referenceAlpha + threshold;
			alphaTest.Z = -1.0f;
			alphaTest.W = 1.0f;
		}
		else if (m_compareFunction == CompareFunction::Equal)
		{
			alphaTest.X = m_referenceAlpha;
			alphaTest.Y = threshold;
			alphaTest.Z = 1.0f;
			alphaTest.W = -1.0f;
		}
		else if (m_compareFunction == CompareFunction::NotEqual)
		{
			alphaTest.X = m_referenceAlpha;
			alphaTest.Y = threshold;
			alphaTest.Z = -1.0f;
			alphaTest.W = 1.0f;
		}
		else if (m_compareFunction == CompareFunction::Never)
		{
			alphaTest.Z = -1.0f;
			alphaTest.W = -1.0f;
		}
		else if (m_compareFunction == CompareFunction::Always)
		{
			alphaTest.Z = 1.0f;
			alphaTest.W = -1.0f;
		}

		m_hlslEffect->GetParameter("AlphaTest")->SetValue(alphaTest);

		// figure out which program to use
		if (m_isVertexColorEnabled)
		{
			if (m_compareFunction == CompareFunction::Equal ||
				m_compareFunction == CompareFunction::NotEqual)
				m_device->SetCurrentEffect(m_hlslEffect, 1);
			else
				m_device->SetCurrentEffect(m_hlslEffect, 0);
		}
		else
		{
			if (m_compareFunction == CompareFunction::Equal ||
				m_compareFunction == CompareFunction::NotEqual)
				m_device->SetCurrentEffect(m_hlslEffect, 3);
			else
				m_device->SetCurrentEffect(m_hlslEffect, 2);
		}
	}
}
}
}

#endif