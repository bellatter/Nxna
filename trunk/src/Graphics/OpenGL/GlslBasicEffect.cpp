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

		glslEffect->CreateProgram("ColorAndTexture", true, (const byte*)vertexResult.c_str(), vertexResult.length(), (const byte*)fragResult.c_str(), fragResult.length());
		glslEffect->CreateProgram("Color", true, (const byte*)vertexResult.c_str(), vertexResult.length(), (const byte*)fragResult.c_str(), fragResult.length());
		glslEffect->CreateProgram("Texture", true, (const byte*)vertexResult.c_str(), vertexResult.length(), (const byte*)fragResult.c_str(), fragResult.length());
		glslEffect->CreateProgram("Nothing", true, (const byte*)vertexResult.c_str(), vertexResult.length(), (const byte*)fragResult.c_str(), fragResult.length());

		// create the dummy technique
		glslEffect->CreateProgram("default", false, nullptr, 0, nullptr, 0);
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