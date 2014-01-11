#ifndef GRAPHICS_GLSLSPRITEEFFECT_H
#define GRAPHICS_GLSLSPRITEEFFECT_H

#include "../SpriteEffect.h"
#include "../SpriteEffectPimpl.h"
#include "GlslEffect.h"

// VC++ gives a stupid warning that doesn't
// apply since we're using pure virtual methods
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4250)
#endif

namespace Nxna
{
namespace Graphics
{
namespace OpenGl
{
	class GlslSpriteEffect : public Pvt::SpriteEffectPimpl
	{
		GlslEffect* m_glslEffect;

	public:
		GlslSpriteEffect(OpenGlDevice* device, GlslEffect* glslEffect);
		virtual ~GlslSpriteEffect() { }
	};
}
}
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif // GRAPHICS_GLSLSPRITEEFFECT_H
