#ifndef NXNA_GRAPHICS_DIRECT3D11_HLSLDUALTEXTUREEFFECT_H
#define NXNA_GRAPHICS_DIRECT3D11_HLSLDUALTEXTUREEFFECT_H

#include "../DualTextureEffect.h"
#include "../DualTextureEffectPimpl.h"
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
	class HlslDualTextureEffect : public Pvt::DualTextureEffectPimpl
	{
		HlslEffect* m_hlslEffect;
		Direct3D11Device* m_device;

	public:
		HlslDualTextureEffect(Direct3D11Device* device, HlslEffect* hlslEffect);
		virtual ~HlslDualTextureEffect() { }

		virtual void Apply(int programIndex) override;
	};
}
}
}

NXNA_ENABLE_OVERRIDE_WARNING

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif // NXNA_GRAPHICS_DIRECT3D11_HLSLDUALTEXTUREEFFECT_H
