#ifndef GRAPHICS_OPENGL_GLSLBASICEFFECT_H
#define GRAPHICS_OPENGL_GLSLBASICEFFECT_H

#include "../BasicEffect.h"
#include "../BasicEffectPimpl.h"
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
	class GlslBasicEffect : public Pvt::BasicEffectPimpl
	{
		GlslEffect* m_glslEffect;

	public:

		GlslBasicEffect(OpenGlDevice* device, GlslEffect* glslEffect);
		virtual ~GlslBasicEffect() { }

		virtual void Apply(int programIndex) override;
	};
}
}
}

NXNA_ENABLE_OVERRIDE_WARNING

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif // GRAPHICS_OPENGL_GLSLBASICEFFECT_H
