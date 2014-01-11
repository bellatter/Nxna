#ifndef NXNA_GRAPHICS_DIRECT3D11_HLSLALPHATESTEFFECT_H
#define NXNA_GRAPHICS_DIRECT3D11_HLSLALPHATESTEFFECT_H


#include "../AlphaTestEffect.h"
#include "../AlphaTestEffectPimpl.h"
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
	class HlslAlphaTestEffect : public Pvt::AlphaTestEffectPimpl
	{
		HlslEffect* m_hlslEffect;
		Direct3D11Device* m_device;

	public:

		HlslAlphaTestEffect(Direct3D11Device* device, HlslEffect* hlslEffect);
		virtual ~HlslAlphaTestEffect() { }

		virtual void Apply() override;
	};
}
}
}

#endif // NXNA_GRAPHICS_DIRECT3D11_HLSLALPHATESTEFFECT_H