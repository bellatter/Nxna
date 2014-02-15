#include "SamplerState.h"

namespace Nxna
{
namespace Graphics
{
	bool SamplerState::m_isInitialized = false;
	SamplerState SamplerState::m_linearClamp;
	SamplerState SamplerState::m_linearWrap;
	SamplerState SamplerState::m_pointClamp;

	void SamplerState::init()
	{
		m_linearClamp.AddressU = TextureAddressMode::Clamp;
		m_linearClamp.AddressV = TextureAddressMode::Clamp;
		m_linearClamp.AddressW = TextureAddressMode::Clamp;

		m_linearWrap.AddressU = TextureAddressMode::Wrap;
		m_linearWrap.AddressV = TextureAddressMode::Wrap;
		m_linearWrap.AddressW = TextureAddressMode::Wrap;

		m_pointClamp.AddressU = TextureAddressMode::Clamp;
		m_pointClamp.AddressV = TextureAddressMode::Clamp;
		m_pointClamp.AddressW = TextureAddressMode::Clamp;
		m_pointClamp.Filter = TextureFilter::Point;
	}
}
}