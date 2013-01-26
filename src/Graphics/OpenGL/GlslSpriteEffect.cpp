#include "GlslSpriteEffect.h"

namespace Nxna
{
namespace Graphics
{
namespace OpenGl
{
	const char* spriteEffectVertexSource =
		"uniform HIGHP mat4 ModelViewProjection; "
		"in vec3 position : POSITION0; "
		"in vec2 texCoords : TEXCOORD0; "
		"in vec4 color: COLOR0; "
		"out vec2 o_diffuseCoords; "
		"out vec4 o_color; "
		"void main() "
		"{ "
		"	gl_Position = ModelViewProjection * vec4(position, 1.0); "
		"	o_diffuseCoords = texCoords; "
		"	o_color = color;\n"
		"}";
	const char* spriteEffectFragmentSource =
		"uniform sampler2D Diffuse; "
		"in HIGHP vec2 o_diffuseCoords; "
		"in HIGHP vec4 o_color;\n"
		"#if VERSION >= 130\n"
		"out vec4 outputColor;\n"
        "#endif\n"
		"void main() "
		"{ \n"
		"#if VERSION < 130\n"
		"	gl_FragColor = texture2D(Diffuse, o_diffuseCoords) * o_color; \n "
		"#else\n"
		"	outputColor = texture(Diffuse, o_diffuseCoords) * o_color; \n"
		"#endif\n"
		"}";

	GlslSpriteEffect::GlslSpriteEffect(OpenGlDevice* device)
		: GlslEffect(device, spriteEffectVertexSource, spriteEffectFragmentSource)
	{
	}
}
}
}

