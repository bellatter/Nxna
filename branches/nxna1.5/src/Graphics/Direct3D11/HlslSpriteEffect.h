#ifndef NXNA_GRAPHICS_DIRECT3D11_HLSLSPRITEEFFECT_H
#define NXNA_GRAPHICS_DIRECT3D11_HLSLSPRITEEFFECT_H

#include "../SpriteEffect.h"
#include "../SpriteEffectPimpl.h"
#include "HlslEffect.h"

// VC++ gives a stupid warning that doesn't
// apply since we're using pure virtual methods
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4250)
#endif

namespace Nxna
{
namespace Graphics
{
namespace Direct3D11
{
	class HlslSpriteEffect : public Pvt::SpriteEffectPimpl
	{
		HlslEffect* m_hlslEffect;

	public:
		HlslSpriteEffect(Direct3D11Device* device, HlslEffect* hlslEffect);
		virtual ~HlslSpriteEffect() { }
	};
}
}
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif // NXNA_GRAPHICS_DIRECT3D11_HLSLSPRITEEFFECT_H
