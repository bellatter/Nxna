#include <cstdio>
#include "GlslAlphaTestEffect.h"
#include "../AlphaTestEffectPimpl.h"
#include "OpenGLDevice.h"

namespace Nxna
{
namespace Graphics
{
namespace OpenGl
{

#include "ShaderSource/AlphaTestEffect.vert.inc"
#include "ShaderSource/AlphaTestEffect.frag.inc"

	GlslAlphaTestEffect::GlslAlphaTestEffect(OpenGlDevice* device, GlslEffect* glslEffect)
		: Pvt::AlphaTestEffectPimpl(glslEffect), m_glslEffect(glslEffect)
	{
		std::string vertexResult, fragResult;
		glslEffect->ProcessSource(AlphaTestEffect_vert, AlphaTestEffect_frag, vertexResult, fragResult);

		char buffer[100];
		sprintf(buffer, "#version %d\n", device->GetGlslVersion());
		std::string versionString = buffer;

		const char* colorAndLess[] = { "#define VERTEXCOLORENABLED\n", "#define LESSGREATER\n"};
		const char* color[] = { "#define VERTEXCOLORENABLED\n" };
		const char* less[] = { "#define LESSGREATER\n" };

		glslEffect->CreateProgram("ColorLessGreater", true, (const byte*)vertexResult.c_str(), vertexResult.length(), (const byte*)fragResult.c_str(), fragResult.length());
		glslEffect->CreateProgram("Color", true, (const byte*)vertexResult.c_str(), vertexResult.length(), (const byte*)fragResult.c_str(), fragResult.length());
		glslEffect->CreateProgram("LessGreater", true, (const byte*)vertexResult.c_str(), vertexResult.length(), (const byte*)fragResult.c_str(), fragResult.length());
		glslEffect->CreateProgram("Equal", true, (const byte*)vertexResult.c_str(), vertexResult.length(), (const byte*)fragResult.c_str(), fragResult.length());

		// create the dummy technique
		glslEffect->CreateProgram("default", false, nullptr, 0, nullptr, 0);
	}

	void GlslAlphaTestEffect::Apply(int programIndex)
	{
		Matrix worldView;
		Matrix worldViewProjection;

		Matrix::Multiply(m_world, m_view, worldView);
		Matrix::Multiply(worldView, m_projection, worldViewProjection);

		m_glslEffect->GetParameter("ModelViewProjection")->SetValue(worldViewProjection.C);

		// calculate the AlphaTest parameter
		const float threshold = 0.5f / 255.0f;
		Vector4 alphaTest;
		if (m_compareFunction == CompareFunction::Less)
		{
			alphaTest.X = m_referenceAlpha - threshold;
			alphaTest.Z = 1.0f;
			alphaTest.W = -1.0f;
		}
		else if (m_compareFunction == CompareFunction::LessEqual)
		{
			alphaTest.X = m_referenceAlpha + threshold;
			alphaTest.Z = 1.0f;
			alphaTest.W = -1.0f;
		}
		else if (m_compareFunction == CompareFunction::GreaterEqual)
		{
			alphaTest.X = m_referenceAlpha - threshold;
			alphaTest.Z = -1.0f;
			alphaTest.W = 1.0f;
		}
		else if (m_compareFunction == CompareFunction::Greater)
		{
			alphaTest.X = m_referenceAlpha + threshold;
			alphaTest.Z = -1.0f;
			alphaTest.W = 1.0f;
		}
		else if (m_compareFunction == CompareFunction::Equal)
		{
			alphaTest.X = m_referenceAlpha;
			alphaTest.Y = threshold;
			alphaTest.Z = 1.0f;
			alphaTest.W = -1.0f;
		}
		else if (m_compareFunction == CompareFunction::NotEqual)
		{
			alphaTest.X = m_referenceAlpha;
			alphaTest.Y = threshold;
			alphaTest.Z = -1.0f;
			alphaTest.W = 1.0f;
		}
		else if (m_compareFunction == CompareFunction::Never)
		{
			alphaTest.Z = -1.0f;
			alphaTest.W = -1.0f;
		}
		else if (m_compareFunction == CompareFunction::Always)
		{
			alphaTest.Z = 1.0f;
			alphaTest.W = -1.0f;
		}

		m_glslEffect->GetParameter("AlphaTest")->SetValue(alphaTest);

		m_glslEffect->ApplyProgram(programIndex);
	}
}
}
}