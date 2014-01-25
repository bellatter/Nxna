#include "DualTextureEffect.h"
#include "DualTextureEffectPimpl.h"

namespace Nxna
{
namespace Graphics
{
	DualTextureEffect::DualTextureEffect(GraphicsDevice* device)
		: Effect(device)
	{
		m_dtePimpl = device->CreateDualTextureEffectPimpl(this, m_pimpl);
	}

	bool DualTextureEffect::IsVertexColorEnabled() { return m_dtePimpl->IsVertexColorEnabled(); }
	void DualTextureEffect::IsVertexColorEnabled(bool enabled) { m_dtePimpl->IsVertexColorEnabled(enabled); }

	void DualTextureEffect::SetWorld(const Matrix& matrix) { m_dtePimpl->SetWorld(matrix); }
	void DualTextureEffect::SetView(const Matrix& matrix) { m_dtePimpl->SetView(matrix); }
	void DualTextureEffect::SetProjection(const Matrix& matrix) { m_dtePimpl->SetProjection(matrix); }

	void DualTextureEffect::SetTexture(Texture2D* texture) { m_dtePimpl->SetTexture(texture); }
	void DualTextureEffect::SetTexture2(Texture2D* texture) { m_dtePimpl->SetTexture2(texture); }

	void DualTextureEffect::OnApply()
	{
		if (m_dtePimpl->IsVertexColorEnabled())
			m_dtePimpl->Apply(0);
		else
			m_dtePimpl->Apply(1);
	}
}
}