#include "BasicEffect.h"
#include "IEffectPimpl.h"

#include "Effects/BasicEffect.inc"

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

		GetParameter("ModelViewProjection")->SetValue(m_finalTransform.C);

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