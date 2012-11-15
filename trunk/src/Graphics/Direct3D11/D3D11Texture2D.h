#ifndef NXNA_GRAPHICS_DIRECT3D11_D3D11TEXTURE2D_H
#define NXNA_GRAPHICS_DIRECT3D11_D3D11TEXTURE2D_H

#include "../Texture2D.h"
#include "../SamplerState.h"

NXNA_DISABLE_OVERRIDE_WARNING

namespace Nxna
{
namespace Graphics
{
	class GraphicsDevice;

namespace Direct3D11
{
	class D3D11Texture2D : public Texture2D
	{
		void* m_shaderResourceView;
		bool m_hasMipmaps;
		SamplerState m_samplerState;

	public:

		D3D11Texture2D(GraphicsDevice* device, int width, int height, SurfaceFormat format);

		virtual void SetData(int level, byte* pixels, int length) override;

		virtual void SetSamplerState(const SamplerState* state) override;

	private:
		static int convertAddressMode(TextureAddressMode mode);
	};
}
}
}

NXNA_ENABLE_OVERRIDE_WARNING

#endif // NXNA_GRAPHICS_DIRECT3D11_D3D11TEXTURE2D_H