#include <cstdio>
#include "GlslAlphaTestEffect.h"
#include "OpenGLDevice.h"

namespace Nxna
{
namespace Graphics
{
namespace OpenGl
{

#include "ShaderSource/AlphaTestEffect.vert.inc"
#include "ShaderSource/AlphaTestEffect.frag.inc"

	GlslAlphaTestEffect::GlslAlphaTestEffect(OpenGlDevice* device)
		: GlslEffect(device)
	{
		m_finalTransformDirty = true;
		m_isVertexColorEnabled = false;
		m_alpha = 0;
		m_referenceAlpha = 0;
		m_compareFunction = CompareFunction::Greater;

		Matrix::GetIdentity(m_world);
		Matrix::GetIdentity(m_view);
		Matrix::GetIdentity(m_projection);

		std::string vertexResult, fragResult;
		ProcessSource(AlphaTestEffect_vert, AlphaTestEffect_frag, vertexResult, fragResult);

		char buffer[100];
		sprintf(buffer, "#version %d\n", device->GetGlslVersion());
		std::string versionString = buffer;

		const char* colorAndLess[] = { "#define VERTEXCOLORENABLED\n", "#define LESSGREATER\n"};
		const char* color[] = { "#define VERTEXCOLORENABLED\n" };
		const char* less[] = { "#define LESSGREATER\n" };

		CreateProgram(vertexResult, fragResult, colorAndLess, 2);
		CreateProgram(vertexResult, fragResult, color, 1);
		CreateProgram(vertexResult, fragResult, less, 1);
		CreateProgram(vertexResult, fragResult, nullptr, 0);
	}

	void GlslAlphaTestEffect::SetTexture(Texture2D* texture)
	{
		GetParameter("Diffuse")->SetValue(texture);
	}

	void GlslAlphaTestEffect::Apply()
	{
		Matrix worldView;
		Matrix worldViewProjection;

		Matrix::Multiply(m_world, m_view, worldView);
		Matrix::Multiply(worldView, m_projection, worldViewProjection);

		GetParameter("ModelViewProjection")->SetValue(worldViewProjection.C);

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

		GetParameter("AlphaTest")->SetValue(alphaTest);

		// figure out which program to use
		if (m_isVertexColorEnabled)
		{
			if (m_compareFunction == CompareFunction::Equal ||
				m_compareFunction == CompareFunction::NotEqual)
				ApplyProgram(1);
			else
				ApplyProgram(0);
		}
		else
		{
			if (m_compareFunction == CompareFunction::Equal ||
				m_compareFunction == CompareFunction::NotEqual)
				ApplyProgram(3);
			else
				ApplyProgram(2);
		}
	}
}
}
}