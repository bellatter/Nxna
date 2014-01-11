#ifndef NXNA_GRAPHICS_ALPHATESTEFFECTPIMPL_H
#define NXNA_GRAPHICS_ALPHATESTEFFECTPIMPL_H

#include "IEffectPimpl.h"
#include "Effect.h"
#include "../Matrix.h"

namespace Nxna
{
namespace Graphics
{
namespace Pvt
{
	class AlphaTestEffectPimpl
	{
	protected:
		Matrix m_world;
		Matrix m_view;
		Matrix m_projection;
		Matrix m_finalTransform;
		bool m_finalTransformDirty;

		bool m_isVertexColorEnabled;

		float m_alpha;
		float m_referenceAlpha;
		CompareFunction m_compareFunction;

		IEffectPimpl* m_pimpl;

	public:

		virtual ~AlphaTestEffectPimpl() { }

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

		void SetReferenceAlpha(float alpha) { m_referenceAlpha = alpha; }
		float GetReferenceAlpha() { return m_referenceAlpha; }

		void SetAlpha(float alpha) { m_alpha = alpha; }
		float GetAlpha() { return m_alpha; }

		void SetAlphaFunction(CompareFunction function) { m_compareFunction = function; }
		CompareFunction GetAlphaFunction() { return m_compareFunction; }

		virtual void Apply() = 0;

	protected:
		AlphaTestEffectPimpl(IEffectPimpl* pimpl)
		{ 
			m_pimpl = pimpl; 

			m_finalTransformDirty = true;
			m_isVertexColorEnabled = false;

			m_alpha = 0;
			m_referenceAlpha = 0;
			m_compareFunction = CompareFunction::Greater;

			Matrix::GetIdentity(m_world);
			Matrix::GetIdentity(m_view);
			Matrix::GetIdentity(m_projection);
		}
	};
}
}
}

#endif // NXNA_GRAPHICS_BASICEFFECTPIMPL_H
