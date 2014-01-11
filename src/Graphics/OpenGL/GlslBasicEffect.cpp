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
#include "ShaderSource/BasicEffect.vert.inc"
#include "ShaderSource/BasicEffect.frag.inc"

	GlslBasicEffect::GlslBasicEffect(OpenGlDevice* device, GlslEffect* glslEffect)
		: Pvt::BasicEffectPimpl(glslEffect), m_glslEffect(glslEffect)
	{
		std::string vertexResult, fragResult;
		glslEffect->ProcessSource(BasicEffect_vert, BasicEffect_frag, vertexResult, fragResult);

		char buffer[100];
		sprintf(buffer, "#version %d\n", device->GetGlslVersion());
		std::string versionString = buffer;

		const char* colorAndTexture[] = { "#define VERTEXCOLORENABLED\n", "#define TEXTUREENABLED\n" };
		const char* color[] = { "#define VERTEXCOLORENABLED\n" };
		const char* texture[] = { "#define TEXTUREENABLED\n" };

		glslEffect->CreateProgram(vertexResult, fragResult, colorAndTexture, 2);
		glslEffect->CreateProgram(vertexResult, fragResult, color, 1);
		glslEffect->CreateProgram(vertexResult, fragResult, texture, 1);
		glslEffect->CreateProgram(vertexResult, fragResult, nullptr, 0);
	}

	void GlslBasicEffect::Apply()
	{
		Matrix worldView;
		Matrix worldViewProjection;

		Matrix::Multiply(m_world, m_view, worldView);
		Matrix::Multiply(worldView, m_projection, worldViewProjection);

		m_glslEffect->GetParameter("ModelViewProjection")->SetValue(worldViewProjection.C);

		// figure out which program to use
		if (m_isVertexColorEnabled && m_isTextureEnabled)
			m_glslEffect->ApplyProgram(0);
		else if (m_isVertexColorEnabled)
			m_glslEffect->ApplyProgram(1);
		else if (m_isTextureEnabled)
			m_glslEffect->ApplyProgram(2);
		else
			m_glslEffect->ApplyProgram(3);
	}
}
}
}