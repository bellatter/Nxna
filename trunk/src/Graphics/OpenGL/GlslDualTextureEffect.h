#ifndef GRAPHICS_OPENGL_GLSLDUALTEXTUREEFFECT_H
#define GRAPHICS_OPENGL_GLSLDUALTEXTUREEFFECT_H

#include "../DualTextureEffect.h"
#include "../DualTextureEffectPimpl.h"
#include "GlslEffect.h"

// VC++ gives a stupid warning that doesn't
// apply since we're using pure virtual methods
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4250)
#endif

NXNA_DISABLE_OVERRIDE_WARNING

namespace Nxna
{
namespace Graphics
{
namespace OpenGl
{
	class GlslDualTextureEffect : public Pvt::DualTextureEffectPimpl
	{
		GlslEffect* m_glslEffect;

	public:
		GlslDualTextureEffect(OpenGlDevice* device, GlslEffect* glslEffect);
		virtual ~GlslDualTextureEffect() { }

		virtual void Apply() override;
	};
}
}
}

NXNA_ENABLE_OVERRIDE_WARNING

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif // GRAPHICS_OPENGL_GLSLDUALTEXTUREEFFECT_H