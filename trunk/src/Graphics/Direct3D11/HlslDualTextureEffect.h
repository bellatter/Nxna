#ifndef NXNA_GRAPHICS_DIRECT3D11_HLSLDUALTEXTUREEFFECT_H
#define NXNA_GRAPHICS_DIRECT3D11_HLSLDUALTEXTUREEFFECT_H

#include "../DualTextureEffect.h"
#include "HlslEffect.h"

// VC++ gives a stupid warning that doesn't
// apply since we're using pure virtual methods
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4250)
#endif

NXNA_DISABLE_OVERRIDE_WARNING

namespace Nxna
{
namespace Graphics
{
namespace Direct3D11
{
	class HlslDualTextureEffect : public DualTextureEffect, public HlslEffect
	{
		Matrix m_world;
		Matrix m_view;
		Matrix m_projection;
		Matrix m_finalTransform;
		bool m_finalTransformDirty;

		bool m_isVertexColorEnabled;

	public:
		HlslDualTextureEffect(Direct3D11Device* device);

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
		virtual void SetTexture2(Texture2D* texture) override;

		virtual void Apply() override;
	};
}
}
}

NXNA_ENABLE_OVERRIDE_WARNING

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif // NXNA_GRAPHICS_DIRECT3D11_HLSLDUALTEXTUREEFFECT_H
