#include "../../NxnaConfig.h"

#if defined NXNA_PLATFORM_WIN32

#include "D3D11Texture2D.h"
#include "Direct3D11Device.h"
#include "d3d11.h"

namespace Nxna
{
namespace Graphics
{
namespace Direct3D11
{
	D3D11Texture2D::D3D11Texture2D(GraphicsDevice* device, int width, int height, SurfaceFormat format)
		: Texture2D(device)
	{
		m_width = width;
		m_height = height;
		m_format = format;
		m_hasMipmaps = false;
	}

	void D3D11Texture2D::SetData(int level, byte* pixels, int length)
	{
		D3D11_TEXTURE2D_DESC desc;
		desc.ArraySize = 1;
		if (m_format == SurfaceFormat_Color)
			desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		else if (m_format == SurfaceFormat_Dxt1)
			desc.Format = DXGI_FORMAT_BC1_UNORM;
		else if (m_format == SurfaceFormat_Dxt3)
			desc.Format = DXGI_FORMAT_BC2_UNORM;
		else if (m_format == SurfaceFormat_Dxt5)
			desc.Format = DXGI_FORMAT_BC3_UNORM;
		desc.Width = m_width;
		desc.Height = m_height;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = pixels;
		initData.SysMemPitch = 4 * m_width;

		ID3D11Texture2D* texture;
		if (FAILED(static_cast<ID3D11Device*>(static_cast<Direct3D11Device*>(m_device)->GetDevice())->CreateTexture2D(&desc, &initData, &texture)) || texture == nullptr)
			throw GraphicsException("Unable to create texture");
		
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        ZeroMemory(&srvDesc, sizeof(srvDesc));
		srvDesc.Format = desc.Format;
		srvDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = desc.MipLevels;

		static_cast<ID3D11Device*>(static_cast<Direct3D11Device*>(m_device)->GetDevice())->CreateShaderResourceView(texture, &srvDesc, (ID3D11ShaderResourceView**)&m_shaderResourceView);

		texture->Release();
	}

	void D3D11Texture2D::SetSamplerState(const SamplerState* state)
	{
		// TODO
	}
}
}
}

#endif