#ifndef NXNA_GRAPHICS_SAMPLERSTATE_H
#define NXNA_GRAPHICS_SAMPLERSTATE_H

#include "../NxnaConfig.h"

namespace Nxna
{
namespace Graphics
{
	NXNA_ENUM(TextureAddressMode)
		Clamp,
		Mirror,
		Wrap
	END_NXNA_ENUM(TextureAddressMode)

	NXNA_ENUM(TextureFilter)
		Linear,
		Point,
		Anisotropic,
		LinearMipPoint,
		PointMipLinear,
		MinLinearMagPointMipLinear,
		MinLinearMagPointMipPoint,
		MinPointMagLinearMipLinear,
		MinPointMagLinearMipPoint
	END_NXNA_ENUM(TextureFilter)

	class SamplerState
	{
		friend class GraphicsDevice;

		static SamplerState m_linearClamp;
		static SamplerState m_linearWrap;
		static SamplerState m_pointClamp;

		void* m_handle;

	public:

		SamplerState()
		{
			AddressU = TextureAddressMode::Wrap;
			AddressV = TextureAddressMode::Wrap;
			AddressW = TextureAddressMode::Wrap;
			Filter = TextureFilter::Linear;
			MaxAnisotropy = 4;
			MaxMipLevel = 0;
			MipMapLevelOfDetailBias = 0;

			m_handle = nullptr;
		}

		TextureAddressMode AddressU;
		TextureAddressMode AddressV;
		TextureAddressMode AddressW;
		TextureFilter Filter;
		int MaxAnisotropy;
		int MaxMipLevel;
		float MipMapLevelOfDetailBias;

		static const SamplerState* GetLinearClamp()
		{
			if (m_isInitialized == false)
				init();

			return &m_linearClamp;
		}

		static const SamplerState* GetLinearWrap()
		{
			if (m_isInitialized == false)
				init();

			return &m_linearWrap;
		}

		static const SamplerState* GetPointClamp()
		{
			if (m_isInitialized == false)
				init();

			return &m_pointClamp;
		}

	private:
		static bool m_isInitialized;
		static void init();
	};
}
}

#endif // NXNA_GRAPHICS_SAMPLERSTATE_H
