#include <cassert>
#include <cstdio>
#include "OpenGLDevice.h"
#include "GlslBasicEffect.h"

namespace Nxna
{
namespace Graphics
{
namespace OpenGl
{
	// this shader code is so messy. Apologies.
	const char* basicEffectVertexSrc = 
		"uniform HIGHP mat4 ModelViewProjection;\n"

		"in vec4 position : POSITION0;\n"
		"#if defined TEXTUREENABLED\n"
		"in vec2 texCoords : TEXCOORD0;\n"
		"out vec2 o_diffuseCoords;\n"
		"#endif\n"
		"#if defined VERTEXCOLORENABLED\n"
		"in vec4 color : COLOR0;\n"
		"out vec4 o_color;\n"
		"#endif\n"
		
		"void main()\n"
		"{\n"
		"	gl_Position = ModelViewProjection * position;\n"
		"#if defined TEXTUREENABLED\n"
		"	o_diffuseCoords = texCoords;\n"
		"#endif\n"
		"#if defined VERTEXCOLORENABLED\n"
		"	o_color = color;\n"
		"#endif\n"
		"}";

	const char* basicEffectFragmentSrc =
		"#if defined TEXTUREENABLED\n"
		"uniform sampler2D Diffuse;\n"
		"in HIGHP vec2 o_diffuseCoords;\n"
		"#endif\n"

		"#if defined VERTEXCOLORENABLED\n"
		"in HIGHP vec4 o_color;\n"
		"#endif\n"

        "#if __VERSION__ >= 130\n"
		"out HIGHP vec4 outputColor;\n"
        "#endif\n"
		"void main()\n"
		"{\n"
		"	HIGHP vec4 finalColor;\n"
		"#if defined TEXTUREENABLED\n\n"
		"#if __VERSION__ < 130\n"
		"	finalColor = texture2D(Diffuse, o_diffuseCoords);\n"
		"#else\n"
		"	finalColor = texture(Diffuse, o_diffuseCoords);\n"
		"#endif\n"
		"#if defined VERTEXCOLORENABLED\n"
		"	finalColor *= o_color;\n"
		"#endif\n"
		"#elif defined VERTEXCOLORENABLED\n"
		"	finalColor = o_color;\n"
		"#endif\n"
		
		"#if __VERSION__ < 130\n"
		"	gl_FragColor = finalColor;\n"
		"#else\n"
		"	outputColor = finalColor;\n"
        "#endif\n"

		"}";

	GlslBasicEffect::GlslBasicEffect(OpenGlDevice* device)
		: GlslEffect(device)
	{
		m_finalTransformDirty = true;
		m_isTextureEnabled = false;
		m_isVertexColorEnabled = false;

		Matrix::GetIdentity(m_world);
		Matrix::GetIdentity(m_view);
		Matrix::GetIdentity(m_projection);

		std::string vertexResult, fragResult;
		ProcessSource(basicEffectVertexSrc, basicEffectFragmentSrc, vertexResult, fragResult);

		char buffer[100];
		sprintf(buffer, "#version %d\n", device->GetGlslVersion());
		std::string versionString = buffer;

		const char* colorAndTexture[] = { "#define VERTEXCOLORENABLED\n", "#define TEXTUREENABLED\n" };
		const char* color[] = { "#define VERTEXCOLORENABLED\n" };
		const char* texture[] = { "#define TEXTUREENABLED\n" };

		CreateProgram(vertexResult, fragResult, colorAndTexture, 2);
		CreateProgram(vertexResult, fragResult, color, 1);
		CreateProgram(vertexResult, fragResult, texture, 1);
		CreateProgram(vertexResult, fragResult, nullptr, 0);
	}

	void GlslBasicEffect::SetTexture(Texture2D* texture)
	{
		GetParameter("Diffuse")->SetValue(texture);
	}

	void GlslBasicEffect::Apply()
	{
		Matrix worldView;
		Matrix worldViewProjection;

		Matrix::Multiply(m_world, m_view, worldView);
		Matrix::Multiply(worldView, m_projection, worldViewProjection);

		GetParameter("ModelViewProjection")->SetValue(worldViewProjection.C);

		// figure out which program to use
		if (m_isVertexColorEnabled && m_isTextureEnabled)
			ApplyProgram(0);
		else if (m_isVertexColorEnabled)
			ApplyProgram(1);
		else if (m_isTextureEnabled)
			ApplyProgram(2);
		else
			ApplyProgram(3);
	}
}
}
}