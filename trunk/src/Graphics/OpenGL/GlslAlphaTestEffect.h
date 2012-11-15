#ifndef NXNA_GRAPHICS_OPENGL_GLSLALPHATESTEFFECT_H
#define NXNA_GRAPHICS_OPENGL_GLSLALPHATESTEFFECT_H

#include "../AlphaTestEffect.h"
#include "GlslEffect.h"

NXNA_DISABLE_OVERRIDE_WARNING

namespace Nxna
{
namespace Graphics
{
namespace OpenGl
{
	class GlslAlphaTestEffect : public GlslEffect, public AlphaTestEffect
	{
		Matrix m_world;
		Matrix m_view;
		Matrix m_projection;
		Matrix m_finalTransform;
		bool m_finalTransformDirty;

		bool m_isVertexColorEnabled;
		float m_alpha;
		float m_referenceAlpha;
		CompareFunction m_compareFunction;

	public:
		GlslAlphaTestEffect(OpenGlDevice* device);

		virtual bool IsVertexColorEnabled() override { return m_isVertexColorEnabled; }
		virtual void IsVertexColorEnabled(bool enabled) override { m_isVertexColorEnabled = enabled; }

		virtual void SetWorld(const Matrix& matrix) override
		{
			m_world = matrix; m_finalTransformDirty = true;
		};

		virtual void SetView(const Matrix& matrix) override
		{
			m_view = matrix; m_finalTransformDirty = true;
		};

		virtual void SetProjection(const Matrix& matrix) override
		{
			m_projection = matrix; m_finalTransformDirty = true;
		};

		virtual void SetTexture(Texture2D* texture) override;

		virtual void SetReferenceAlpha(float alpha) override { m_referenceAlpha = alpha; }
		virtual float GetReferenceAlpha() override { return m_referenceAlpha; }

		virtual void SetAlpha(float alpha) override { m_alpha = alpha; }
		virtual float GetAlpha() override { return m_alpha; }

		virtual void SetAlphaFunction(CompareFunction function) override { m_compareFunction = function; }
		virtual CompareFunction GetAlphaFunction() override { return m_compareFunction; }

		virtual void Apply() override;
	};
}
}
}

NXNA_ENABLE_OVERRIDE_WARNING

#endif // NXNA_GRAPHICS_OPENGL_GLSLALPHATESTEFFECT_H