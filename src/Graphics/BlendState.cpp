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
		m_additive.ColorSourceBlend = Blend_SourceAlpha;
		m_additive.AlphaSourceBlend = Blend_SourceAlpha;
		m_additive.ColorDestinationBlend = Blend_One;
		m_additive.AlphaDestinationBlend = Blend_One;

		m_alphaBlend.ColorSourceBlend = Blend_One;
		m_alphaBlend.AlphaSourceBlend = Blend_One;
		m_alphaBlend.ColorDestinationBlend = Blend_InverseSourceAlpha;
		m_alphaBlend.AlphaDestinationBlend = Blend_InverseSourceAlpha;

		m_nonPreMultiplied.ColorSourceBlend = Blend_SourceAlpha;
		m_nonPreMultiplied.AlphaSourceBlend = Blend_SourceAlpha;
		m_nonPreMultiplied.ColorDestinationBlend = Blend_InverseSourceAlpha;
		m_nonPreMultiplied.AlphaDestinationBlend = Blend_InverseSourceAlpha;

		m_opaque.ColorSourceBlend = Blend_One;
		m_opaque.AlphaSourceBlend = Blend_One;
		m_opaque.ColorDestinationBlend = Blend_Zero;
		m_opaque.AlphaDestinationBlend = Blend_Zero;

		m_isInitialized = true;
	}
}
}