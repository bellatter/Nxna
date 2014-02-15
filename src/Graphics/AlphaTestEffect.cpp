#include "AlphaTestEffect.h"
#include "IEffectPimpl.h"

#include "Effects/AlphaTestEffect.inc"

namespace Nxna
{
namespace Graphics
{
	AlphaTestEffect::AlphaTestEffect(GraphicsDevice* device)
		: Effect(device, (byte*)AlphaTestEffect_bytecode, sizeof(AlphaTestEffect_bytecode))
	{
		m_vertexColorEnabled = false;
		m_finalTransformDirty = true;

		Matrix::GetIdentity(m_world);
		Matrix::GetIdentity(m_view);
		Matrix::GetIdentity(m_projection);

		m_transform = GetParameter("ModelViewProjection");
		m_diffuse = GetParameter("Diffuse");
		m_diffuseColor = GetParameter("DiffuseColor");
		m_alphaTest = GetParameter("AlphaTest");

		m_compareFunction = CompareFunction::Greater;
		m_alpha = 1.0f;
		m_referenceAlpha = 0;
	}

	void AlphaTestEffect::OnApply()
	{
		if (m_finalTransformDirty)
		{
			Matrix worldView;
			Matrix worldViewProjection;

			Matrix::Multiply(m_world, m_view, worldView);
			Matrix::Multiply(worldView, m_projection, worldViewProjection);

			m_transform->SetValue(worldViewProjection.C);

			m_finalTransformDirty = false;
		}

		m_diffuseColor->SetValue(Vector4(1.0f, 1.0f, 1.0f, 1.0f));

		float fRefAlpha = (float)m_referenceAlpha / 255.0f;

		// calculate the AlphaTest parameter
		const float threshold = 0.5f / 255.0f;
		Vector4 alphaTest;
		if (m_compareFunction == CompareFunction::Less)
		{
			alphaTest.X = fRefAlpha - threshold;
			alphaTest.Z = 1.0f;
			alphaTest.W = -1.0f;
		}
		else if (m_compareFunction == CompareFunction::LessEqual)
		{
			alphaTest.X = fRefAlpha + threshold;
			alphaTest.Z = 1.0f;
			alphaTest.W = -1.0f;
		}
		else if (m_compareFunction == CompareFunction::GreaterEqual)
		{
			alphaTest.X = fRefAlpha - threshold;
			alphaTest.Z = -1.0f;
			alphaTest.W = 1.0f;
		}
		else if (m_compareFunction == CompareFunction::Greater)
		{
			alphaTest.X = fRefAlpha + threshold;
			alphaTest.Z = -1.0f;
			alphaTest.W = 1.0f;
		}
		else if (m_compareFunction == CompareFunction::Equal)
		{
			alphaTest.X = fRefAlpha;
			alphaTest.Y = threshold;
			alphaTest.Z = 1.0f;
			alphaTest.W = -1.0f;
		}
		else if (m_compareFunction == CompareFunction::NotEqual)
		{
			alphaTest.X = fRefAlpha;
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

		m_alphaTest->SetValue(alphaTest);

		if (m_vertexColorEnabled)
		{
			if (m_compareFunction == CompareFunction::Equal ||
				m_compareFunction == CompareFunction::NotEqual)
				m_pimpl->Apply(1);
			else
				m_pimpl->Apply(0);
		}
		else
		{
			if (m_compareFunction == CompareFunction::Equal ||
				m_compareFunction == CompareFunction::NotEqual)
				m_pimpl->Apply(3);
			else
				m_pimpl->Apply(2);
		}
	}
}
}