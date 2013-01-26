#include <cstdio>
#include "GlslDualTextureEffect.h"
#include "OpenGLDevice.h"

namespace Nxna
{
namespace Graphics
{
namespace OpenGl
{
	const char* dualTextureEffectVertexSrc = 
		"uniform HIGHP mat4 ModelViewProjection;\n"

		"in vec4 position : POSITION0;\n"
		"in vec2 texCoords1 : TEXCOORD0;\n"
		"in vec2 texCoords2 : TEXCOORD1;\n"
		"out vec2 o_diffuseCoords1;\n"
		"out vec2 o_diffuseCoords2;\n"
		"#if defined VERTEXCOLORENABLED\n"
		"in vec4 color : COLOR0;\n"
		"out vec4 o_color;\n"
		"#endif\n"

		"void main()\n"
		"{\n"
		"	gl_Position = ModelViewProjection * position;\n"
		"	o_diffuseCoords1 = texCoords1;\n"
		"	o_diffuseCoords2 = texCoords2;\n"
		"#if defined VERTEXCOLORENABLED\n"
		"	o_color = color;\n"
		"#endif\n"
		"}";

	const char* dualTextureEffectFragmentSrc =
		"uniform sampler2D Diffuse;\n"
		"uniform sampler2D Diffuse2;\n"

		"in HIGHP vec2 o_diffuseCoords1;\n"
		"in HIGHP vec2 o_diffuseCoords2;\n"

		"#if defined VERTEXCOLORENABLED\n"
		"in HIGHP vec4 o_color;\n"
		"#endif\n"

        "#if __VERSION__ >= 130\n"
		"out HIGHP vec4 outputColor;\n"
        "#endif\n"
		"void main()\n"
		"{\n"
		"	HIGHP vec4 finalColor;\n"
		"#if __VERSION__ < 130\n"
		"	HIGHP vec4 color1 = texture2D(Diffuse, o_diffuseCoords1); \n"
		"	HIGHP vec4 color2 = texture2D(Diffuse2, o_diffuseCoords2); \n"
		"#else\n"
		"	vec4 color1 = texture(Diffuse, o_diffuseCoords1); \n"
		"	vec4 color2 = texture(Diffuse2, o_diffuseCoords2); \n"
		"#endif\n"
		"#if defined VERTEXCOLORENABLED\n"
		"	finalColor = vec4(color1.rgb * color2.rgb * 2.0, color1.a * color2.a) * o_color;\n"
		"#else\n"
		"	finalColor = vec4(color1.rgb * color2.rgb * 2.0, color1.a * color2.a);\n"
		"#endif\n"
		"#if __VERSION__ < 130\n"
		"	gl_FragColor = finalColor;\n"
		"#else\n"
		"	outputColor = finalColor;\n"
        "#endif\n"
		"}";

	GlslDualTextureEffect::GlslDualTextureEffect(OpenGlDevice* device)
		: GlslEffect(device)
	{
		m_isVertexColorEnabled = false;

		std::string vertexResult, fragResult;
		ProcessSource(dualTextureEffectVertexSrc, dualTextureEffectFragmentSrc, vertexResult, fragResult);

		const char* color[] = { "#define VERTEXCOLORENABLED\n" };

		CreateProgram(vertexResult, fragResult, color, 1);
		CreateProgram(vertexResult, fragResult, nullptr, 0);
		
		// HACK: sometimes OpenGL reports the sampler uniforms in a different
		// order than they exist in the shader source, which breaks this effect.
		// Go back and make sure the order is correct.
		if (m_textureParams[0]->Name != "Diffuse")
		{
			EffectParameter* dummy = m_textureParams[0];
			m_textureParams[0] = m_textureParams[1];
			m_textureParams[1] = dummy;
		}
	}

	void GlslDualTextureEffect::SetTexture(Texture2D* texture)
	{
		GetParameter("Diffuse")->SetValue(texture);
	}

	void GlslDualTextureEffect::SetTexture2(Texture2D* texture)
	{
		GetParameter("Diffuse2")->SetValue(texture);
	}

	void GlslDualTextureEffect::Apply()
	{
		Matrix worldView;
		Matrix worldViewProjection;

		Matrix::Multiply(m_world, m_view, worldView);
		Matrix::Multiply(worldView, m_projection, worldViewProjection);

		GetParameter("ModelViewProjection")->SetValue(worldViewProjection.C);

		if (m_isVertexColorEnabled)
			ApplyProgram(0);
		else
			ApplyProgram(1);
	}
}
}
}