#include <cstdio>
#include "GlslDualTextureEffect.h"
#include "OpenGLDevice.h"

namespace Nxna
{
namespace Graphics
{
namespace OpenGl
{
	
#include "ShaderSource/DualTextureEffect.vert.inc"
#include "ShaderSource/DualTextureEffect.frag.inc"

	GlslDualTextureEffect::GlslDualTextureEffect(OpenGlDevice* device)
		: GlslEffect(device)
	{
		m_isVertexColorEnabled = false;

		std::string vertexResult, fragResult;
		ProcessSource(DualTextureEffect_vert, DualTextureEffect_frag, vertexResult, fragResult);

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