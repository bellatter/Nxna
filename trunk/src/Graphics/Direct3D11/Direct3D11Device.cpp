#include "Direct3D11Device.h"
#include "D3D11Texture2D.h"
#include "D3D11VertexBuffer.h"
#include "D3D11IndexBuffer.h"
#include "HlslSpriteEffect.h"
#include <d3dcommon.h>
#include <d3d11.h>

namespace Nxna
{
namespace Graphics
{
namespace Direct3D11
{
	Direct3D11Device::Direct3D11Device()
	{
	}

	void Direct3D11Device::OnWindowCreated(void* window, int width, int height)
	{
		const char* error = "Unable to initialize Direct3D";

		IDXGIFactory* factory;
		HRESULT result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
		if (FAILED(result))
			throw GraphicsException(error, __FILE__, __LINE__);

		IDXGIAdapter* adapter;
		result = factory->EnumAdapters(0, &adapter);
		if (FAILED(result))
			throw GraphicsException(error, __FILE__, __LINE__);

		DXGI_ADAPTER_DESC adapterDesc;
		if (FAILED(adapter->GetDesc(&adapterDesc)))
			throw GraphicsException(error, __FILE__, __LINE__);

		IDXGIOutput* adapterOutput;
		if (FAILED(adapter->EnumOutputs(0, &adapterOutput)))
			throw GraphicsException(error, __FILE__, __LINE__);

		unsigned int numModes;
		if (FAILED(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL)))
			throw GraphicsException(error, __FILE__, __LINE__);

		DXGI_MODE_DESC* displayModeList = new DXGI_MODE_DESC[numModes];
		if (FAILED(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList)))
		{
			delete[] displayModeList;
			throw GraphicsException(error, __FILE__, __LINE__);
		}

		int numerator = 0, denominator = 0;
		for(int i = 0; i < numModes; i++)
		{
			if(displayModeList[i].Width == (unsigned int)width)
			{
				if(displayModeList[i].Height == (unsigned int)height)
				{
					numerator = displayModeList[i].RefreshRate.Numerator;
					denominator = displayModeList[i].RefreshRate.Denominator;
					break;
				}
			}
		}

		delete[] displayModeList;
		adapterOutput->Release();
		adapter->Release();
		factory->Release();
		
		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
		swapChainDesc.BufferCount = 1;
		swapChainDesc.BufferDesc.Width = width;
		swapChainDesc.BufferDesc.Height = height;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.OutputWindow = (HWND)window;
		swapChainDesc.Windowed = true;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;

		D3D_FEATURE_LEVEL featureLevel[] = {D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0};

		 result = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, featureLevel, 3, 
			D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, nullptr, &m_deviceContext);

		 if (FAILED(result))
			 throw GraphicsException(error, __FILE__, __LINE__);

		 ID3D11Texture2D* backBufferPtr;
		 if (FAILED(m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr)))
			 throw GraphicsException(error, __FILE__, __LINE__);

		 ID3D11RenderTargetView* m_renderTargetView;
		 if (FAILED(m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView)))
			throw GraphicsException(error, __FILE__, __LINE__);

	}

	CullMode Direct3D11Device::GetRasterizerState() 
	{
		return CullMode::CullMode_CullClockwiseFace;
	}

	void Direct3D11Device::SetRasterizerState(const RasterizerState* state) {}

	DepthStencilState Direct3D11Device::GetDepthStencilState()
	{
		return *DepthStencilState::GetDefault();
	}

	void Direct3D11Device::SetDepthStencilState(const DepthStencilState* state) { }

	void Direct3D11Device::SetIndices(const IndexBuffer* indices) { }

	void Direct3D11Device::Clear(const Color& c) { }
	void Direct3D11Device::Clear(ClearOptions options, const Color& c, float depth, int stencil) { }
	Viewport Direct3D11Device::GetViewport()
	{
		return Viewport();
	}

	void Direct3D11Device::SetViewport(const Viewport& viewport) { }

	void Direct3D11Device::DrawIndexedPrimitives(PrimitiveType primitiveType, int baseVertex, int minVertexIndex, int numVertices, int startIndex, int primitiveCount) { }
	void Direct3D11Device::DrawPrimitives(PrimitiveType primitiveType, int startVertex, int primitiveCount) { }
	void Direct3D11Device::DrawUserIndexedPrimitives(PrimitiveType primitiveType, void* data, int numVertices, int* indices, int primitiveCount, const VertexDeclaration* vertexDeclaration) { }
	void Direct3D11Device::DrawUserIndexedPrimitives(PrimitiveType primitiveType, void* data, int numVertices, short* indices, int primitiveCount, const VertexDeclaration* vertexDeclaration) { }
	void Direct3D11Device::DrawUserPrimitives(PrimitiveType primitiveType, void* data, int primitiveCount, const VertexDeclaration* vertexDeclaration) { }

	void Direct3D11Device::SetVertexBuffer(const VertexBuffer* vertexBuffer) { }
	void Direct3D11Device::SetBlendState(const BlendState* blendState) { }

	void Direct3D11Device::Present() { }

	Texture2D* Direct3D11Device::CreateTexture(int width, int height)
	{
		return new D3D11Texture2D(this, width, height, SurfaceFormat_Color);
	}
	
	Texture2D* Direct3D11Device::CreateTexture(int width, int height, SurfaceFormat format) 
	{
		return new D3D11Texture2D(this, width, height, format);
	}

	BasicEffect* Direct3D11Device::CreateBasicEffect()
	{
		return nullptr;
	}

	SpriteEffect* Direct3D11Device::CreateSpriteEffect() 
	{
		return new HlslSpriteEffect(this);
	}

	DualTextureEffect* Direct3D11Device::CreateDualTextureEffect()
	{
		return nullptr;
	}

	AlphaTestEffect* Direct3D11Device::CreateAlphaTestEffect()
	{
		return nullptr;
	}

	VertexBuffer* Direct3D11Device::CreateVertexBuffer(const VertexDeclaration* vertexDeclaration, int vertexCount, BufferUsage usage)
	{
		return new D3D11VertexBuffer(this, vertexDeclaration, vertexCount, usage);
	}

	DynamicVertexBuffer* Direct3D11Device::CreateDynamicVertexBuffer(const VertexDeclaration* vertexDeclaration, int vertexCount, BufferUsage usage)
	{
		return new D3D11DynamicVertexBuffer(this, vertexDeclaration, vertexCount, usage);
	}

	IndexBuffer* Direct3D11Device::CreateIndexBuffer(IndexElementSize elementSize)
	{
		return new D3D11IndexBuffer(m_device, elementSize);
	}

	void Direct3D11Device::GetBackBufferData(void* data) { }

}
}
}