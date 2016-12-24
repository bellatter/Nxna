#ifndef GRAPHICS_DUALTEXTUREEFFECT_H
#define GRAPHICS_DUALTEXTUREEFFECT_H

#include "Effect.h"
#include "GraphicsDevice.h"
#include "../Matrix.h"

NXNA_DISABLE_OVERRIDE_WARNING

namespace Nxna
{
namespace Graphics
{
	class DualTextureEffect : public Effect
	{
		bool m_vertexColorEnabled;
		Matrix m_world;
		Matrix m_view;
		Matrix m_projection;
		Matrix m_finalTransform;
		bool m_finalTransformDirty;

	public:
		DualTextureEffect(GraphicsDevice* device);
		virtual ~DualTextureEffect() {}

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
			GetParameter("Diffuse")->SetValue(texture);
		}

		void SetTexture2(Texture2D* texture)
		{
			GetParameter("Diffuse2")->SetValue(texture);
		}
	
	protected:

		virtual void OnApply() override;
	};
}
}

NXNA_ENABLE_OVERRIDE_WARNING

#endif // GRAPHICS_DUALTEXTUREEFFECT_H