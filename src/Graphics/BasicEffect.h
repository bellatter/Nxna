#ifndef GRAPHICS_BASICEFFECT_H
#define GRAPHICS_BASICEFFECT_H

#include "Effect.h"
#include "GraphicsDevice.h"
#include "../Matrix.h"

NXNA_DISABLE_OVERRIDE_WARNING

namespace Nxna
{
namespace Graphics
{
	class GraphicsDevice;
	class Texture2D;

	class BasicEffect : public Effect
	{
		bool m_textureEnabled;
		bool m_vertexColorEnabled;

		Matrix m_world;
		Matrix m_view;
		Matrix m_projection;
		Matrix m_finalTransform;
		bool m_finalTransformDirty;

		Vector3 m_diffuse;
		float m_alpha;
		bool m_colorDirty;

		EffectParameter* m_transformParameter;
		EffectParameter* m_diffuseParameter;
		EffectParameter* m_diffuseColorParameter;

	public:

		BasicEffect(GraphicsDevice* device);
		virtual ~BasicEffect() {}

		bool IsTextureEnabled() { return m_textureEnabled; }
		void IsTextureEnabled(bool enabled) { m_textureEnabled = enabled; }

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

		void SetTexture(Texture2D* texture);

		void SetDiffuse(const Nxna::Vector3& color)
		{
			m_diffuse = color;
			m_colorDirty = true;
		}

		void SetAlpha(float alpha)
		{
			m_alpha = alpha;
			m_colorDirty = true;
		}

	protected:

		virtual void OnApply() override;
	};
}
}

NXNA_ENABLE_OVERRIDE_WARNING

#endif // GRAPHICS_BASICEFFECT_H