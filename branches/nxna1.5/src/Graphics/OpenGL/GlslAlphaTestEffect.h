#ifndef NXNA_GRAPHICS_OPENGL_GLSLALPHATESTEFFECT_H
#define NXNA_GRAPHICS_OPENGL_GLSLALPHATESTEFFECT_H

#include "../AlphaTestEffect.h"
#include "../AlphaTestEffectPimpl.h"
#include "GlslEffect.h"

NXNA_DISABLE_OVERRIDE_WARNING

namespace Nxna
{
namespace Graphics
{
namespace OpenGl
{
	class GlslAlphaTestEffect : public Pvt::AlphaTestEffectPimpl
	{
		GlslEffect* m_glslEffect;

	public:
		GlslAlphaTestEffect(OpenGlDevice* device, GlslEffect* glslEffect);
		virtual ~GlslAlphaTestEffect() { }

		virtual void Apply(int programIndex) override;
	};
}
}
}

NXNA_ENABLE_OVERRIDE_WARNING

#endif // NXNA_GRAPHICS_OPENGL_GLSLALPHATESTEFFECT_H