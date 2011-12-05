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
	const char* basicEffectSrc = 
		"#vertex\n"

		"uniform {highp} mat4 ModelViewProjection;\n"

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
		"}\n"

		"#fragment\n"

		"#if defined TEXTUREENABLED\n"
		"uniform sampler2D Diffuse;\n"
		"in {highp} vec2 o_diffuseCoords;\n"
		"#endif\n"

		"#if defined VERTEXCOLORENABLED\n"
		"in {highp} vec4 o_color;\n"
		"#endif\n"

        "#if __VERSION__ >= 130\n"
		"out {highp} vec4 outputColor;\n"
        "#endif\n"
		"void main()\n"
		"{\n"
		"	{highp} vec4 finalColor;\n"
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
		ProcessSource(basicEffectSrc, vertexResult, fragResult);

		char buffer[100];
		sprintf(buffer, "#version %d\n", device->GetGlslVersion());
		std::string versionString = buffer;

		CreateProgram(vertexResult, fragResult, (versionString + "#define VERTEXCOLORENABLED\n#define TEXTUREENABLED\n").c_str());
		CreateProgram(vertexResult, fragResult, (versionString + "#define VERTEXCOLORENABLED\n").c_str());
		CreateProgram(vertexResult, fragResult, (versionString + "#define TEXTUREENABLED\n").c_str());
		CreateProgram(vertexResult, fragResult, versionString.c_str());
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