#include "BasicEffect.h"
#include "BasicEffectPimpl.h"

namespace Nxna
{
namespace Graphics
{
	BasicEffect::BasicEffect(GraphicsDevice* device)
		: Effect(device)
	{
		m_bePimpl = device->CreateBasicEffectPimpl(this, m_pimpl);
	}

	bool BasicEffect::IsTextureEnabled() { return m_bePimpl->IsTextureEnabled(); }
	void BasicEffect::IsTextureEnabled(bool enabled) { m_bePimpl->IsTextureEnabled(enabled); }

	bool BasicEffect::IsVertexColorEnabled() { return m_bePimpl->IsVertexColorEnabled(); }
	void BasicEffect::IsVertexColorEnabled(bool enabled) { m_bePimpl->IsVertexColorEnabled(enabled); }

	void BasicEffect::SetWorld(const Matrix& matrix) { m_bePimpl->SetWorld(matrix); }
	void BasicEffect::SetView(const Matrix& matrix) { m_bePimpl->SetView(matrix); }
	void BasicEffect::SetProjection(const Matrix& matrix) { m_bePimpl->SetProjection(matrix); }

	void BasicEffect::SetTexture(Texture2D* texture) { m_bePimpl->SetTexture(texture); }

	void BasicEffect::Apply()
	{
		m_bePimpl->Apply();
	}
}
}