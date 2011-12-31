#ifndef NXNA_GRAPHICS_SAMPLERSTATE_H
#define NXNA_GRAPHICS_SAMPLERSTATE_H

#include "../NxnaConfig.h"

namespace Nxna
{
namespace Graphics
{
	enum TextureAddressMode
	{
		TextureAddressMode_Clamp,
		TextureAddressMode_Mirror,
		TextureAddressMode_Wrap
	};

	enum TextureFilter
	{
		TextureFilter_Linear,
		TextureFilter_Point,
		TextureFilter_Anisotropic,
		TextureFilter_LinearMipPoint,
		TextureFilter_PointMipLinear,
		TextureFilter_MinLinearMagPointMipLinear,
		TextureFilter_MinLinearMagPointMipPoint,
		TextureFilter_MinPointMagLinearMipLinear,
		TextureFilter_MinPointMagLinearMipPoint
	};

	class SamplerState
	{
		static SamplerState m_linearClamp;
		static SamplerState m_linearWrap;

		void* m_handle;

	public:

		SamplerState()
		{
			AddressU = TextureAddressMode_Wrap;
			AddressV = TextureAddressMode_Wrap;
			AddressW = TextureAddressMode_Wrap;
			Filter = TextureFilter_Linear;
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

	private:
		static bool m_isInitialized;
		static void init();
	};
}
}

#endif // NXNA_GRAPHICS_SAMPLERSTATE_H
