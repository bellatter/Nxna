#ifndef NXNA_GRAPHICS_SPRITEEFFECTPIMPL_H
#define NXNA_GRAPHICS_SPRITEEFFECTPIMPL_H

#include "IEffectPimpl.h"
#include "Effect.h"
#include "../Matrix.h"

namespace Nxna
{
namespace Graphics
{
namespace Pvt
{
	class SpriteEffectPimpl
	{
		IEffectPimpl* m_pimpl;

	public:
		virtual ~SpriteEffectPimpl() { }

	protected:
		SpriteEffectPimpl(IEffectPimpl* pimpl)
		{
			m_pimpl = pimpl;
		}
	};
}
}
}

#endif // NXNA_GRAPHICS_SPRITEEFFECTPIMPL_H
