#include "SpriteEffect.h"
#include "SpriteEffectPimpl.h"
#include "GraphicsDevice.h"

namespace Nxna
{
namespace Graphics
{
	SpriteEffect::SpriteEffect(GraphicsDevice* device)
		: Effect(device)
	{
		m_sePimpl = device->CreateSpriteEffectPimpl(this, m_pimpl);
	}
}
}