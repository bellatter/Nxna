#ifndef NXNA_GRAPHICS_ALPHATESTEFFECT_H
#define NXNA_GRAPHICS_ALPHATESTEFFECT_H

#include "Effect.h"
#include "GraphicsDevice.h"
#include "../Matrix.h"

NXNA_DISABLE_OVERRIDE_WARNING

namespace Nxna
{
namespace Graphics
{
	class AlphaTestEffect : public Effect
	{
		bool m_vertexColorEnabled;

		Matrix m_world;
		Matrix m_view;
		Matrix m_projection;
		Matrix m_finalTransform;
		bool m_finalTransformDirty;

		EffectParameter* m_transform;
		EffectParameter* m_diffuse;
		EffectParameter* m_diffuseColor;
		EffectParameter* m_alphaTest;

		CompareFunction m_compareFunction;
		int m_referenceAlpha;
		float m_alpha;

	public:

		AlphaTestEffect(GraphicsDevice* device);
		virtual ~AlphaTestEffect() {}

		bool IsVertexColorEnabled() { return m_vertexColorEnabled; }
		void IsVertexColorEnabled(bool enabled) { m_vertexColorEnabled = enabled; }

		void SetWorld(const Matrix& matrix)
		{
			m_world = matrix;
			m_finalTransformDirty = true;
		}

		void SetView(const Matrix& matrix)
		{
			m_view = matrix;
			m_finalTransformDirty = true;
		}

		void SetProjection(const Matrix& matrix)
		{
			m_projection = matrix;
			m_finalTransformDirty = true;
		}

		void SetTexture(Texture2D* texture)
		{
			m_diffuse->SetValue(texture);
		}

		void SetReferenceAlpha(int alpha)
		{
			m_referenceAlpha = alpha;
		}

		int GetReferenceAlpha()
		{
			return m_referenceAlpha;
		}

		void SetAlpha(float alpha)
		{
			m_alpha = alpha;
		}

		float GetAlpha()
		{
			return m_alpha;
		}

		void SetAlphaFunction(CompareFunction function)
		{
			m_compareFunction = function;
		}

		CompareFunction GetAlphaFunction()
		{
			return m_compareFunction;
		}

	protected:
		virtual void OnApply() override;
	};
}
}

NXNA_ENABLE_OVERRIDE_WARNING

#endif // NXNA_GRAPHICS_ALPHATESTEFFECT_H