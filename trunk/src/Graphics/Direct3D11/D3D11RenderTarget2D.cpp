#include <d3d11.h>
#include "D3D11RenderTarget2D.h"
#include "D3D11Texture2D.h"
#include "Direct3D11Device.h"
#include "D3D11Utils.h"

namespace Nxna
{
namespace Graphics
{
namespace Direct3D11
{
	D3D11RenderTarget2D::D3D11RenderTarget2D(Direct3D11Device* device, D3D11Texture2D* texture, int width, int height, SurfaceFormat preferredFormat, DepthFormat preferredDepthFormat, int preferredMultiSampleCount, RenderTargetUsage usage)
	{
		texture->MakeRenderTarget();

		ID3D11Device* d3dDevice = static_cast<ID3D11Device*>(static_cast<Direct3D11Device*>(device)->GetDevice());

		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
		ZeroMemory(&rtvDesc, sizeof(rtvDesc));
		rtvDesc.Format = D3D11Utils::ConvertSurfaceFormat(preferredFormat);
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D.MipSlice = 0;

		if (FAILED(d3dDevice->CreateRenderTargetView(texture->GetTexture(), &rtvDesc, &m_renderTargetView)))
			throw GraphicsException("Unable to create texture");

		if (preferredDepthFormat == DepthFormat::None)
		{
			m_depthStencilView = nullptr;
			m_depthStencilBuffer = nullptr;
		}
		else
		{
			D3D11_TEXTURE2D_DESC depthBufferDesc;
			ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
			depthBufferDesc.Width = width;
			depthBufferDesc.Height = height;
			depthBufferDesc.MipLevels = 1;
			depthBufferDesc.ArraySize = 1;
			depthBufferDesc.Format = D3D11Utils::ConvertDepthFormat(preferredDepthFormat);
			depthBufferDesc.SampleDesc.Count = 1;
			depthBufferDesc.SampleDesc.Quality = 0;
			depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			depthBufferDesc.CPUAccessFlags = 0;
			depthBufferDesc.MiscFlags = 0;

			if (FAILED(d3dDevice->CreateTexture2D(&depthBufferDesc, nullptr, &m_depthStencilBuffer)))
				throw GraphicsException("Unable to create render target depth buffer", __FILE__, __LINE__);

			D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
			ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
			depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			depthStencilViewDesc.Texture2D.MipSlice = 0;
			if (FAILED(d3dDevice->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView)))
				throw GraphicsException("Unable to create render target depth view", __FILE__, __LINE__);
		}
	}

	D3D11RenderTarget2D::~D3D11RenderTarget2D()
	{
		if (m_depthStencilView != nullptr)
			m_depthStencilView->Release();
		if (m_depthStencilBuffer != nullptr)
			m_depthStencilBuffer->Release();
		if (m_renderTargetView != nullptr)
			m_renderTargetView->Release();
	}
}
}
}
