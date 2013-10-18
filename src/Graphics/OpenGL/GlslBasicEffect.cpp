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
		ProcessSource(BasicEffect_vert, BasicEffect_frag, vertexResult, fragResult);

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