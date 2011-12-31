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
		m_linearClamp.AddressU = TextureAddressMode_Clamp;
		m_linearClamp.AddressV = TextureAddressMode_Clamp;
		m_linearClamp.AddressW = TextureAddressMode_Clamp;

		m_linearWrap.AddressU = TextureAddressMode_Wrap;
		m_linearWrap.AddressV = TextureAddressMode_Wrap;
		m_linearWrap.AddressW = TextureAddressMode_Wrap;
	}
}
}