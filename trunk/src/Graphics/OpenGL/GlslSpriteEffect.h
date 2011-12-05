#ifndef GRAPHICS_GLSLSPRITEEFFECT_H
#define GRAPHICS_GLSLSPRITEEFFECT_H

#include "../SpriteEffect.h"
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
	class GlslSpriteEffect : public SpriteEffect, public GlslEffect
	{
	public:
		GlslSpriteEffect(OpenGlDevice* device);
	};
}
}
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif // GRAPHICS_GLSLSPRITEEFFECT_H
