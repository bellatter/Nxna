#include "GlslSpriteEffect.h"

namespace Nxna
{
namespace Graphics
{
namespace OpenGl
{

#include "ShaderSource/SpriteEffect.vert.inc"
#include "ShaderSource/SpriteEffect.frag.inc"

	GlslSpriteEffect::GlslSpriteEffect(OpenGlDevice* device, GlslEffect* glslEffect)
		: Pvt::SpriteEffectPimpl(glslEffect), m_glslEffect(glslEffect)
	{
		std::string vertexResult, fragResult;
		glslEffect->ProcessSource(SpriteEffect_vert, SpriteEffect_frag, vertexResult, fragResult);
		glslEffect->CreateProgram("default", false, vertexResult, fragResult, nullptr, 0);
	}
}
}
}

