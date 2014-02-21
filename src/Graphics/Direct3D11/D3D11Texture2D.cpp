#include "../../NxnaConfig.h"
#include "../../MathHelper.h"

#if !defined NXNA_DISABLE_D3D11

#include "D3D11Texture2D.h"
#include "Direct3D11Device.h"
#include "D3D11Utils.h"
#include "d3d11.h"

namespace Nxna
{
namespace Graphics
{
namespace Direct3D11
{
	D3D11Texture2D::D3D11Texture2D(GraphicsDevice* device, int width, int height, SurfaceFormat format)
	{
		m_device = device;
		m_width = width;
		m_height = height;
		m_format = format;
		m_shaderResourceView = nullptr;
		m_texture = nullptr;
		m_hasMipmaps = false;
	}

	D3D11Texture2D::~D3D11Texture2D()
	{
		if (m_shaderResourceView != nullptr)
		{
			m_shaderResourceView->Release();
			m_shaderResourceView = nullptr;
		}

		if (m_texture != nullptr)
		{
			m_texture->Release();
			m_texture = nullptr;
		}
	}

	void D3D11Texture2D::SetData(int level, byte* pixels, int length)
	{
		// TODO: get mipmapping working
		if (level > 0) return;
		
		int mipWidth = m_width >> level;
		int mipHeight = m_height >> level;

		createTexture(false, pixels, length);
	}

	void D3D11Texture2D::MakeRenderTarget()
	{
		createTexture(true, nullptr, 0);
	}

	void D3D11Texture2D::createTexture(bool isRenderTarget, byte* pixels, int length)
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));

		desc.ArraySize = 1;
		desc.Format = D3D11Utils::ConvertSurfaceFormat(m_format);
		desc.Width = m_width;
		desc.Height = m_height;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | (isRenderTarget ? D3D11_BIND_RENDER_TARGET : 0);
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		if (pixels != nullptr)
		{
			D3D11_SUBRESOURCE_DATA initData;
			initData.pSysMem = pixels;
			if (m_format == SurfaceFormat::Color)
				initData.SysMemPitch = 4 * m_width;
			else
			{
				int numBlocks = Math::Max(1, m_width / 4);
				initData.SysMemPitch = numBlocks * (m_format == SurfaceFormat::Dxt1 ? 8 : 16);
			}
			
			if (FAILED(static_cast<ID3D11Device*>(static_cast<Direct3D11Device*>(m_device)->GetDevice())->CreateTexture2D(&desc, &initData, &m_texture)) || m_texture == nullptr)
				throw GraphicsException("Unable to create texture");
		}
		else
		{
			if (FAILED(static_cast<ID3D11Device*>(static_cast<Direct3D11Device*>(m_device)->GetDevice())->CreateTexture2D(&desc, nullptr, &m_texture)) || m_texture == nullptr)
				throw GraphicsException("Unable to create texture");
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof(srvDesc));
		srvDesc.Format = desc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = desc.MipLevels;

		if (FAILED(static_cast<ID3D11Device*>(static_cast<Direct3D11Device*>(m_device)->GetDevice())->CreateShaderResourceView(m_texture, &srvDesc, &m_shaderResourceView)))
			throw GraphicsException("Unable to create texture");
	}
}
}
}

#endif