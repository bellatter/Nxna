#include "AlphaTestEffect.h"
#include "AlphaTestEffectPimpl.h"

namespace Nxna
{
namespace Graphics
{
	AlphaTestEffect::AlphaTestEffect(GraphicsDevice* device)
		: Effect(device)
	{
		m_atePimpl = device->CreateAlphaTestEffectPimpl(this, m_pimpl);
	}

	bool AlphaTestEffect::IsVertexColorEnabled() { return m_atePimpl->IsVertexColorEnabled(); }
	void AlphaTestEffect::IsVertexColorEnabled(bool enabled) { m_atePimpl->IsVertexColorEnabled(enabled); }

	void AlphaTestEffect::SetWorld(const Matrix& matrix) { m_atePimpl->SetWorld(matrix); }
	void AlphaTestEffect::SetView(const Matrix& matrix) { m_atePimpl->SetView(matrix); }
	void AlphaTestEffect::SetProjection(const Matrix& matrix) { m_atePimpl->SetProjection(matrix); }

	void AlphaTestEffect::SetTexture(Texture2D* texture) { m_atePimpl->SetTexture(texture); }

	void AlphaTestEffect::Apply()
	{
		m_atePimpl->Apply();
	}
}
}