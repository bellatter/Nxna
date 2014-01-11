#ifndef NXNA_GRAPHICS_DUALTEXTUREEFFECTPIMPL_H
#define NXNA_GRAPHICS_DUALTEXTUREEFFECTPIMPL_H

#include "IEffectPimpl.h"
#include "../Matrix.h"

namespace Nxna
{
namespace Graphics
{
namespace Pvt
{
	class DualTextureEffectPimpl
	{
	protected:
		Matrix m_world;
		Matrix m_view;
		Matrix m_projection;
		Matrix m_finalTransform;
		bool m_finalTransformDirty;

		bool m_isTextureEnabled;
		bool m_isVertexColorEnabled;

		IEffectPimpl* m_pimpl;

	public:
		virtual ~DualTextureEffectPimpl() {}

		bool IsTextureEnabled() { return m_isTextureEnabled; }
		void IsTextureEnabled(bool enabled) { m_isTextureEnabled = enabled; }

		bool IsVertexColorEnabled() { return m_isVertexColorEnabled; }
		void IsVertexColorEnabled(bool enabled) { m_isVertexColorEnabled = enabled; }

		void SetWorld(const Matrix& matrix)
		{
			m_world = matrix; m_finalTransformDirty = true;
		};

		void SetView(const Matrix& matrix)
		{
			m_view = matrix; m_finalTransformDirty = true;
		};

		void SetProjection(const Matrix& matrix)
		{
			m_projection = matrix; m_finalTransformDirty = true;
		};

		void SetTexture(Texture2D* texture)
		{
			m_pimpl->GetParameter("Diffuse")->SetValue(texture);
		}

		void SetTexture2(Texture2D* texture)
		{
			m_pimpl->GetParameter("Diffuse2")->SetValue(texture);
		}

		virtual void Apply() = 0;

	protected:
		DualTextureEffectPimpl(IEffectPimpl* pimpl)
		{ 
			m_pimpl = pimpl; 

			m_finalTransformDirty = true;
			m_isTextureEnabled = false;
			m_isVertexColorEnabled = false;

			Matrix::GetIdentity(m_world);
			Matrix::GetIdentity(m_view);
			Matrix::GetIdentity(m_projection);
		}
	};
}
}
}

#endif // NXNA_GRAPHICS_DUALTEXTUREEFFECTPIMPL_H
