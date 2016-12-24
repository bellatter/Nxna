#ifndef NXNA_GRAPHICS_DIRECT3D11_HLSLBASICEFFECT_H
#define NXNA_GRAPHICS_DIRECT3D11_HLSLBASICEFFECT_H

#include "../BasicEffect.h"
#include "../BasicEffectPimpl.h"
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
	class HlslBasicEffect : public Pvt::BasicEffectPimpl
	{
		HlslEffect* m_hlslEffect;
		Direct3D11Device* m_device;

	public:
		HlslBasicEffect(Direct3D11Device* device, HlslEffect* hlslEffect);
		virtual ~HlslBasicEffect() { }

		virtual void Apply(int programIndex) override;
	};
}
}
}

NXNA_ENABLE_OVERRIDE_WARNING

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif // NXNA_GRAPHICS_DIRECT3D11_HLSLBASICEFFECT_H
