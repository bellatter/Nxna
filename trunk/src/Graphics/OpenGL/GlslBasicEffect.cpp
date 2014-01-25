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

		glslEffect->CreateProgram("ColorAndTexture", true, vertexResult, fragResult, colorAndTexture, 2);
		glslEffect->CreateProgram("Color", true, vertexResult, fragResult, color, 1);
		glslEffect->CreateProgram("Texture", true, vertexResult, fragResult, texture, 1);
		glslEffect->CreateProgram("Nothing", true, vertexResult, fragResult, nullptr, 0);

		glslEffect->CreateDummyTechnique();
	}

	void GlslBasicEffect::Apply(int programIndex)
	{
		Matrix worldView;
		Matrix worldViewProjection;

		Matrix::Multiply(m_world, m_view, worldView);
		Matrix::Multiply(worldView, m_projection, worldViewProjection);

		m_glslEffect->GetParameter("ModelViewProjection")->SetValue(worldViewProjection.C);

		m_glslEffect->ApplyProgram(programIndex);
	}
}
}
}