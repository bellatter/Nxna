#include "SamplerState.h"

namespace Nxna
{
namespace Graphics
{
	bool SamplerState::m_isInitialized = false;
	SamplerState SamplerState::m_linearClamp;
	SamplerState SamplerState::m_linearWrap;

	void SamplerState::init()
	{
		m_linearClamp.AddressU = TextureAddressMode::Clamp;
		m_linearClamp.AddressV = TextureAddressMode::Clamp;
		m_linearClamp.AddressW = TextureAddressMode::Clamp;

		m_linearWrap.AddressU = TextureAddressMode::Wrap;
		m_linearWrap.AddressV = TextureAddressMode::Wrap;
		m_linearWrap.AddressW = TextureAddressMode::Wrap;
	}
}
}