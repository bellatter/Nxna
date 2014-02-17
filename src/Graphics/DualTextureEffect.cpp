#include "DualTextureEffect.h"
#include "IEffectPimpl.h"

#ifdef NXNA_PLATFORM_WIN32
#include "Effects/DualTextureEffect.inc"
#else
#include "Effects/DualTextureEffect_nohlsl.inc"
#endif

namespace Nxna
{
namespace Graphics
{
	DualTextureEffect::DualTextureEffect(GraphicsDevice* device)
		: Effect(device, (byte*)DualTextureEffect_bytecode, sizeof(DualTextureEffect_bytecode))
	{
		m_vertexColorEnabled = false;
		m_finalTransformDirty = true;
	}

	void DualTextureEffect::OnApply()
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

		if (m_vertexColorEnabled)
			m_pimpl->Apply(0);
		else
			m_pimpl->Apply(1);
	}
}
}