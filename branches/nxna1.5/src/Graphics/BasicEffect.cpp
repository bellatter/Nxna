#include "BasicEffect.h"
#include "IEffectPimpl.h"

#ifdef NXNA_PLATFORM_WIN32
#include "Effects/BasicEffect.inc"
#else
#include "Effects/BasicEffect_nohlsl.inc"
#endif

namespace Nxna
{
namespace Graphics
{
	BasicEffect::BasicEffect(GraphicsDevice* device)
		: Effect(device, (byte*)BasicEffect_bytecode, sizeof(BasicEffect_bytecode))
	{
		m_textureEnabled = false;
		m_vertexColorEnabled = false;
		m_finalTransformDirty = true;
		m_colorDirty = true;

		m_world = Matrix::Identity;
		m_view = Matrix::Identity;
		m_projection = Matrix::Identity;

		m_diffuse.X = m_diffuse.Y = m_diffuse.Z = 1.0f;
		m_alpha = 1.0f;

		m_transformParameter = GetParameter("ModelViewProjection");
		m_diffuseParameter = GetParameter("Diffuse");
		m_diffuseColorParameter = GetParameter("DiffuseColor");
	}

	void BasicEffect::SetTexture(Texture2D* texture)
	{
		assert(m_diffuseParameter != nullptr);
		assert(m_diffuseParameter->GetType() == EffectParameterType::Texture2D);

		m_diffuseParameter->SetValue(texture);
	}

	void BasicEffect::OnApply()
	{
		if (m_finalTransformDirty)
		{
			Matrix worldView;
			Matrix worldViewProjection;

			Matrix::Multiply(m_world, m_view, worldView);
			Matrix::Multiply(worldView, m_projection, m_finalTransform);

			m_finalTransformDirty = false;
		}

		m_transformParameter->SetValue(m_finalTransform.C);

		if (m_colorDirty)
		{
			Nxna::Vector4 color;
			color.X = m_diffuse.X * m_alpha;
			color.Y = m_diffuse.Y * m_alpha;
			color.Z = m_diffuse.Z * m_alpha;
			color.W = m_alpha;

			m_diffuseColorParameter->SetValue(color);

			m_colorDirty = false;
		}


		// determine which to apply
		if (m_vertexColorEnabled && m_textureEnabled)
			m_pimpl->Apply(3);
		else if (m_vertexColorEnabled)
			m_pimpl->Apply(2);
		else if (m_textureEnabled)
			m_pimpl->Apply(1);
		else
			m_pimpl->Apply(0);
	}
}
}