#ifndef NXNA_GRAPHICS_DIRECT3D11_D3D11RENDERTARGET2D_H
#define NXNA_GRAPHICS_DIRECT3D11_D3D11RENDERTARGET2D_H

#include "../RenderTarget2D.h"
#include "../IRenderTarget2DPimpl.h"
#include <d3d11.h>

namespace Nxna
{
namespace Graphics
{
namespace Direct3D11
{
	class Direct3D11Device;
	class D3D11Texture2D;

	class D3D11RenderTarget2D : public Pvt::IRenderTarget2DPimpl
	{
		ID3D11RenderTargetView* m_renderTargetView;
		ID3D11DepthStencilView* m_depthStencilView;
		ID3D11Texture2D* m_depthStencilBuffer;

	public:
		D3D11RenderTarget2D(Direct3D11Device* device, D3D11Texture2D* texture, int width, int height, SurfaceFormat preferredFormat, DepthFormat preferredDepthFormat, int preferredMultiSampleCount, RenderTargetUsage usage);
		virtual ~D3D11RenderTarget2D();

		virtual bool FlipBeforeUse() override { return false; }

		ID3D11RenderTargetView* GetRenderTargetView() { return m_renderTargetView; }
		ID3D11DepthStencilView* GetDepthStencilView() { return m_depthStencilView; }
	};
}
}
}

#endif // NXNA_GRAPHICS_DIRECT3D11_D3D11RENDERTARGET2D_H
