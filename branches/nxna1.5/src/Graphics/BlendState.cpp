#include "BlendState.h"

namespace Nxna
{
namespace Graphics
{
	bool BlendState::m_isInitialized = false;
	BlendState BlendState::m_additive;
	BlendState BlendState::m_alphaBlend;
	BlendState BlendState::m_nonPreMultiplied;
	BlendState BlendState::m_opaque;

	void BlendState::init()
	{
		m_additive.ColorSourceBlend = Blend::SourceAlpha;
		m_additive.AlphaSourceBlend = Blend::SourceAlpha;
		m_additive.ColorDestinationBlend = Blend::One;
		m_additive.AlphaDestinationBlend = Blend::One;

		m_alphaBlend.ColorSourceBlend = Blend::One;
		m_alphaBlend.AlphaSourceBlend = Blend::One;
		m_alphaBlend.ColorDestinationBlend = Blend::InverseSourceAlpha;
		m_alphaBlend.AlphaDestinationBlend = Blend::InverseSourceAlpha;

		m_nonPreMultiplied.ColorSourceBlend = Blend::SourceAlpha;
		m_nonPreMultiplied.AlphaSourceBlend = Blend::SourceAlpha;
		m_nonPreMultiplied.ColorDestinationBlend = Blend::InverseSourceAlpha;
		m_nonPreMultiplied.AlphaDestinationBlend = Blend::InverseSourceAlpha;

		m_opaque.ColorSourceBlend = Blend::One;
		m_opaque.AlphaSourceBlend = Blend::One;
		m_opaque.ColorDestinationBlend = Blend::Zero;
		m_opaque.AlphaDestinationBlend = Blend::Zero;

		m_isInitialized = true;
	}
}
}