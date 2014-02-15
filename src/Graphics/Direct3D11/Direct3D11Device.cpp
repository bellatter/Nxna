#include "../../NxnaConfig.h"

#if !defined NXNA_DISABLE_D3D11

#include "Direct3D11Device.h"
#include "D3D11Texture2D.h"
#include "D3D11VertexBuffer.h"
#include "D3D11IndexBuffer.h"
#include "HlslEffect.h"
//#include "HlslBasicEffect.h"
//#include "HlslSpriteEffect.h"
//#include "HlslDualTextureEffect.h"
//#include "HlslAlphaTestEffect.h"
#include "D3D11Utils.h"
#include "../GraphicsDeviceCapabilities.h"
#include "../../Utils.h"
#include <d3dcommon.h>
#include <d3d11.h>
#include <cstdint>

namespace Nxna
{
namespace Graphics
{
namespace Direct3D11
{
	Direct3D11Device::Direct3D11Device()
	{
		m_caps = new GraphicsDeviceCapabilities();

		m_instance = this;
		m_indices = nullptr;
		m_vertices = nullptr;
		m_indexBufferDirty = true;
		m_vertexBufferDirty = true;
		m_blendStateDirty = true;
		m_rasterizerStateDirty = true;

		m_caps->SupportsFullNonPowerOfTwoTextures = true;
		m_caps->SupportsShaders = true;
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

		result = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_DEBUG, featureLevel, 3, 
		D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, nullptr, &m_deviceContext);

		if (FAILED(result))
			throw GraphicsException(error, __FILE__, __LINE__);

		ID3D11Texture2D* backBufferPtr;
		if (FAILED(m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr)))
			throw GraphicsException(error, __FILE__, __LINE__);

		if (FAILED(m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView)))
		throw GraphicsException(error, __FILE__, __LINE__);

		backBufferPtr->Release();

		D3D11_TEXTURE2D_DESC depthBufferDesc;
		ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
		depthBufferDesc.Width = width;
		depthBufferDesc.Height = height;
		depthBufferDesc.MipLevels = 1;
		depthBufferDesc.ArraySize = 1;
		depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthBufferDesc.SampleDesc.Count = 1;
		depthBufferDesc.SampleDesc.Quality = 0;
		depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthBufferDesc.CPUAccessFlags = 0;
		depthBufferDesc.MiscFlags = 0;

		if (FAILED(m_device->CreateTexture2D(&depthBufferDesc, nullptr, &m_depthStencilBuffer)))
			throw GraphicsException(error, __FILE__, __LINE__);

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;
		if (FAILED(m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView)))
			throw GraphicsException(error, __FILE__, __LINE__);

		m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

		// Setup the raster description which will determine how and what polygons will be drawn.
		D3D11_RASTERIZER_DESC rasterDesc;
		rasterDesc.AntialiasedLineEnable = false;
		rasterDesc.CullMode = D3D11_CULL_NONE;
		rasterDesc.DepthBias = 0;
		rasterDesc.DepthBiasClamp = 0.0f;
		rasterDesc.DepthClipEnable = true;
		rasterDesc.FillMode = D3D11_FILL_SOLID;
		rasterDesc.FrontCounterClockwise = false;
		rasterDesc.MultisampleEnable = false;
		rasterDesc.ScissorEnable = false;
		rasterDesc.SlopeScaledDepthBias = 0.0f;

		// Create the rasterizer state from the description we just filled out.
		ID3D11RasterizerState* rasterState; 
		result = m_device->CreateRasterizerState(&rasterDesc, &rasterState);
		if(FAILED(result))
		{
			throw GraphicsException(error, __FILE__, __LINE__);
		}

		// Now set the rasterizer state.
		m_deviceContext->RSSetState(rasterState);

		D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
		ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

		// Set up the description of the stencil state.
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

		depthStencilDesc.StencilEnable = false;
		depthStencilDesc.StencilReadMask = 0xFF;
		depthStencilDesc.StencilWriteMask = 0xFF;

		// Stencil operations if pixel is front-facing.
		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// Stencil operations if pixel is back-facing.
		depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		ID3D11DepthStencilState* dstate;
		if (FAILED(m_device->CreateDepthStencilState(&depthStencilDesc, &dstate)))
			throw GraphicsException(error, __FILE__, __LINE__);

		m_deviceContext->OMSetDepthStencilState(dstate, 1);
	}

	CullMode Direct3D11Device::GetRasterizerState() 
	{
		return CullMode::CullClockwiseFace;
	}

	void Direct3D11Device::SetRasterizerState(const RasterizerState* state)
	{
		m_rasterizerState = *state;
		m_rasterizerStateDirty = true;
	}

	DepthStencilState Direct3D11Device::GetDepthStencilState()
	{
		return m_depthState;
	}

	void Direct3D11Device::SetDepthStencilState(const DepthStencilState* state)
	{
		m_depthState = *state;
		m_depthStencilStateDirty = true;
	}

	void Direct3D11Device::SetIndices(const IndexBuffer* indices)
	{
		m_indices = static_cast<const D3D11IndexBuffer*>(indices->GetPimpl());
		m_indexBufferDirty = true;
	}

	void Direct3D11Device::Clear(const Color& c)
	{
		Clear((ClearOptions::Target | ClearOptions::DepthBuffer | ClearOptions::Stencil), c, 1.0f, 0);
	}

	void Direct3D11Device::Clear(ClearOptions options, const Color& c, float depth, int stencil)
	{
		if ((options & ClearOptions::Target) == ClearOptions::Target)
		{
			float color[4];
			color[0] = c.R / 255.0f;
			color[1] = c.G / 255.0f;
			color[2] = c.B / 255.0f;
			color[3] = c.A / 255.0f;

			m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);
		}

		int flags = 0;
		if ((options & ClearOptions::DepthBuffer) == ClearOptions::DepthBuffer)
			flags |= D3D11_CLEAR_DEPTH;
		if ((options & ClearOptions::Stencil) == ClearOptions::Stencil)
			flags |= D3D11_CLEAR_STENCIL;
		m_deviceContext->ClearDepthStencilView(m_depthStencilView, flags, depth, (byte)stencil);
	}

	Viewport Direct3D11Device::GetViewport()
	{
		D3D11_VIEWPORT vp;
		unsigned int num = 1;
		m_deviceContext->RSGetViewports(&num, &vp);

		Viewport myVP;
		myVP.X = (int)vp.TopLeftX;
		myVP.Y = (int)vp.TopLeftY;
		myVP.Width = (int)vp.Width;
		myVP.Height = (int)vp.Height;

		return myVP;
	}

	void Direct3D11Device::SetViewport(const Viewport& viewport)
	{
		D3D11_VIEWPORT vp;
		vp.TopLeftX = (float)viewport.X;
		vp.TopLeftY = (float)viewport.Y;
		vp.Width = (float)viewport.Width;
		vp.Height = (float)viewport.Height;
		vp.MinDepth = 0;
		vp.MaxDepth = 1.0f;

		m_deviceContext->RSSetViewports(1, &vp);
	}

	void Direct3D11Device::DrawIndexedPrimitives(PrimitiveType primitiveType, int baseVertex, int minVertexIndex, int numVertices, int startIndex, int primitiveCount)
	{
		applyDirtyStates();

		int indexCount;
		D3D11_PRIMITIVE_TOPOLOGY topology;
		if (primitiveType == PrimitiveType::TriangleList)
		{
			indexCount = primitiveCount * 3;
			m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		}
		else if (primitiveType == PrimitiveType::TriangleStrip)
		{
			indexCount = primitiveCount * 3; // FIXME
			m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		}

		m_deviceContext->DrawIndexed(indexCount, startIndex, baseVertex);
	}

	void Direct3D11Device::DrawPrimitives(PrimitiveType primitiveType, int startVertex, int primitiveCount) { }
	void Direct3D11Device::DrawUserIndexedPrimitives(PrimitiveType primitiveType, void* data, int numVertices, int* indices, int primitiveCount, const VertexDeclaration* vertexDeclaration) { }
	void Direct3D11Device::DrawUserIndexedPrimitives(PrimitiveType primitiveType, void* data, int numVertices, short* indices, int primitiveCount, const VertexDeclaration* vertexDeclaration) { }
	void Direct3D11Device::DrawUserPrimitives(PrimitiveType primitiveType, void* data, int primitiveCount, const VertexDeclaration* vertexDeclaration)
	{
	}

	void Direct3D11Device::SetVertexBuffer(const VertexBuffer* vertexBuffer)
	{
		m_vertices = vertexBuffer;
		m_vertexBufferDirty = true;
	}

	void Direct3D11Device::SetBlendState(const BlendState* blendState)
	{
		m_blendState = *blendState;
		m_blendStateDirty = true;
	}

	void Direct3D11Device::SetRenderTarget(RenderTarget2D* renderTarget)
	{
		// TODO
	}

	void Direct3D11Device::Present()
	{
		m_swapChain->Present(0, 0);
	}

	void Direct3D11Device::GetBackBufferData(void* data) { }

	unsigned int Direct3D11Device::CalcShaderHash(const byte* bytecode, int length)
	{
		return Utils::CalcHash(bytecode, length);
	}

	void Direct3D11Device::SetSamplers()
	{
		ID3D11PixelShader* shader;
		m_deviceContext->PSGetShader(&shader, nullptr, nullptr);

		for (int i = 0; i < m_samplers.GetCount(); i++)
		{
			if (m_samplers.IsDirty(i))
			{
				const SamplerState* state = m_samplers.Get(i);
				if (state == nullptr) continue;

				void *const* samplerHandle = GetInternalHandle(state);

				if (*samplerHandle == nullptr)
				{
					D3D11_SAMPLER_DESC sampler;
					sampler.Filter = D3D11Utils::ConvertTextureFilter(state->Filter);
					sampler.AddressU = D3D11Utils::ConvertTextureAddressMode(state->AddressU);
					sampler.AddressV = D3D11Utils::ConvertTextureAddressMode(state->AddressV);
					sampler.AddressW = D3D11Utils::ConvertTextureAddressMode(state->AddressW);
					sampler.MipLODBias = state->MipMapLevelOfDetailBias;
					sampler.MaxAnisotropy = state->MaxAnisotropy;
					sampler.ComparisonFunc =  D3D11_COMPARISON_ALWAYS;
					sampler.BorderColor[0] = 0;
					sampler.BorderColor[1] = 0;
					sampler.BorderColor[2] = 0;
					sampler.BorderColor[3] = 0;
					sampler.MinLOD = 0;
					sampler.MaxLOD = D3D11_FLOAT32_MAX;

					if (FAILED(m_device->CreateSamplerState(&sampler, (ID3D11SamplerState**)samplerHandle)))
						throw GraphicsException("Unable to create sampler state");
				}

				m_deviceContext->PSSetSamplers(i, 1, (ID3D11SamplerState**)samplerHandle);
			}
		}

		m_samplers.MakeClean();
	}

	Pvt::ITexture2DPimpl* Direct3D11Device::CreateTexture2DPimpl(int width, int height, bool mipMap, SurfaceFormat format)
	{
		return new D3D11Texture2D(this, width, height, format);
	}

	Pvt::IRenderTarget2DPimpl* Direct3D11Device::CreateRenderTarget2DPimpl(RenderTarget2D* parentRenderTarget, int width, int height, SurfaceFormat preferredFormat, DepthFormat preferredDepthFormat, int preferredMultiSampleCount, RenderTargetUsage usage)
	{
		return nullptr;
		//return new D3D11RenderTarget2D(this, width, height, foramt);
	}
	
	Pvt::IIndexBufferPimpl* Direct3D11Device::CreateIndexBufferPimpl(IndexElementSize elementSize)
	{
		return new D3D11IndexBuffer(m_device, elementSize);
	}

	Pvt::IVertexBufferPimpl* Direct3D11Device::CreateVertexBufferPimpl(bool dynamic, const VertexDeclaration* vertexDeclaration, int vertexCount, BufferUsage usage)
	{
		return new D3D11VertexBuffer(dynamic, this, vertexDeclaration, vertexCount, usage);
	}

	Pvt::IEffectPimpl* Direct3D11Device::CreateEffectPimpl(Effect* parent)
	{
		return new HlslEffect(this, parent);
	}

	/*Pvt::BasicEffectPimpl* Direct3D11Device::CreateBasicEffectPimpl(BasicEffect* effect, Pvt::IEffectPimpl* pimpl)
	{
		return new HlslBasicEffect(this, static_cast<HlslEffect*>(pimpl));
	}

	Pvt::SpriteEffectPimpl* Direct3D11Device::CreateSpriteEffectPimpl(SpriteEffect* effect, Pvt::IEffectPimpl* pimpl)
	{
		return new HlslSpriteEffect(this, static_cast<HlslEffect*>(pimpl));
	}

	Pvt::DualTextureEffectPimpl* Direct3D11Device::CreateDualTextureEffectPimpl(DualTextureEffect* effect, Pvt::IEffectPimpl* pimpl)
	{
		return new HlslDualTextureEffect(this, static_cast<HlslEffect*>(pimpl));
	}

	Pvt::AlphaTestEffectPimpl* Direct3D11Device::CreateAlphaTestEffectPimpl(AlphaTestEffect* effect, Pvt::IEffectPimpl* pimpl)
	{
		return new HlslAlphaTestEffect(this, static_cast<HlslEffect*>(pimpl));
	}*/

	void Direct3D11Device::applyDirtyStates()
	{
		if (m_blendStateDirty)
		{
			void** internalHandle = GetInternalHandle(&m_blendState);
			if (*internalHandle == nullptr)
			{
				// this is the first time this blend state has been used, so we need to create a new D3D blend state object
				D3D11_BLEND_DESC blendState;
				ZeroMemory(&blendState, sizeof(D3D11_BLEND_DESC));
				blendState.RenderTarget[0].BlendEnable = m_blendState.ColorSourceBlend != Blend::One ||
					m_blendState.AlphaSourceBlend != Blend::One ||
					m_blendState.ColorDestinationBlend != Blend::Zero ||
					m_blendState.AlphaDestinationBlend != Blend::Zero;
				blendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

				blendState.RenderTarget[0].SrcBlend = D3D11Utils::ConvertBlendMode(m_blendState.ColorSourceBlend);
				blendState.RenderTarget[0].DestBlend = D3D11Utils::ConvertBlendMode(m_blendState.ColorDestinationBlend);
				blendState.RenderTarget[0].SrcBlendAlpha = D3D11Utils::ConvertBlendMode(m_blendState.AlphaSourceBlend);
				blendState.RenderTarget[0].DestBlendAlpha = D3D11Utils::ConvertBlendMode(m_blendState.AlphaDestinationBlend);

				blendState.RenderTarget[0].BlendOp = D3D11Utils::ConvertBlendFunc(m_blendState.ColorBlendFunction);
				blendState.RenderTarget[0].BlendOpAlpha = D3D11Utils::ConvertBlendFunc(m_blendState.AlphaBlendFunction);

				if (FAILED(m_device->CreateBlendState(&blendState, (ID3D11BlendState**)internalHandle)))
					throw GraphicsException("Unable to create D3D11 blend state", __FILE__, __LINE__);
			}

			m_deviceContext->OMSetBlendState((ID3D11BlendState*)*internalHandle, nullptr, 0xffffffff);

			m_blendStateDirty = false;
		}

		if (m_depthStencilStateDirty)
		{
			void** internalHandle = GetInternalHandle(&m_depthState);

			if (*internalHandle == nullptr)
			{
				D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
				ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

				// Set up the description of the stencil state.
				depthStencilDesc.DepthEnable = m_depthState.DepthBufferEnable;
				depthStencilDesc.DepthWriteMask = m_depthState.DepthBufferWriteEnable ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
				depthStencilDesc.DepthFunc = D3D11Utils::ConvertComparisonFunc(m_depthState.DepthBufferFunction);

				depthStencilDesc.StencilEnable = m_depthState.StencilEnable;
				depthStencilDesc.StencilReadMask = 0xFF;
				depthStencilDesc.StencilWriteMask = 0xFF;

				// Stencil operations if pixel is front-facing.
				depthStencilDesc.FrontFace.StencilFailOp = D3D11Utils::ConvertStencilOperation(m_depthState.StencilFail);
				depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11Utils::ConvertStencilOperation(m_depthState.StencilDepthBufferFail);
				depthStencilDesc.FrontFace.StencilPassOp = D3D11Utils::ConvertStencilOperation(m_depthState.StencilPass);
				depthStencilDesc.FrontFace.StencilFunc = D3D11Utils::ConvertComparisonFunc(m_depthState.StencilFunction);

				// Stencil operations if pixel is back-facing.
				depthStencilDesc.BackFace.StencilFailOp = depthStencilDesc.FrontFace.StencilFailOp;
				depthStencilDesc.BackFace.StencilDepthFailOp = depthStencilDesc.FrontFace.StencilDepthFailOp;
				depthStencilDesc.BackFace.StencilPassOp = depthStencilDesc.FrontFace.StencilPassOp;
				depthStencilDesc.BackFace.StencilFunc = depthStencilDesc.FrontFace.StencilFunc;

				if (FAILED(m_device->CreateDepthStencilState(&depthStencilDesc, (ID3D11DepthStencilState**)internalHandle)))
					throw GraphicsException("Unable to create D3D11 depth/stencil state", __FILE__, __LINE__);
			}

			m_deviceContext->OMSetDepthStencilState((ID3D11DepthStencilState*)*internalHandle, m_depthState.ReferenceStencil);

			m_depthStencilStateDirty = false;
		}

		if (m_rasterizerStateDirty)
		{
			void** internalHandle = GetInternalHandle(&m_rasterizerState);

			if (*internalHandle == nullptr)
			{
				D3D11_RASTERIZER_DESC rasterizerDesc;
				ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));

				rasterizerDesc.FillMode = D3D11_FILL_SOLID;
				if (m_rasterizerState.TheCullMode == CullMode::CullClockwiseFace)
				{
					rasterizerDesc.CullMode = D3D11_CULL_BACK;
					rasterizerDesc.FrontCounterClockwise = true;
				}
				else if (m_rasterizerState.TheCullMode == CullMode::CullCounterClockwiseFace)
				{
					rasterizerDesc.CullMode = D3D11_CULL_BACK;
					rasterizerDesc.FrontCounterClockwise = false;
				}
				else
				{
					rasterizerDesc.CullMode = D3D11_CULL_NONE;
					rasterizerDesc.FrontCounterClockwise = true;
				}
				rasterizerDesc.DepthClipEnable = true;

				if (FAILED(m_device->CreateRasterizerState(&rasterizerDesc, (ID3D11RasterizerState**)internalHandle)))
					throw GraphicsException("Unable to create D3D11 rasterizer state", __FILE__, __LINE__);
			}

			m_deviceContext->RSSetState((ID3D11RasterizerState*)*internalHandle);

			m_rasterizerStateDirty = false;
		}

		if (m_indexBufferDirty)
		{
			if (m_indices != nullptr)
			{
				IndexElementSize elementSize = m_indices->GetElementSize();
				DXGI_FORMAT format;
				if (elementSize == IndexElementSize::SixteenBits)
					format = DXGI_FORMAT_R16_UINT;
				else
					format = DXGI_FORMAT_R32_UINT;
				m_deviceContext->IASetIndexBuffer(static_cast<ID3D11Buffer*>(m_indices->GetInternalBuffer()), format, 0);
			}

			m_indexBufferDirty = false;
		}

		if (m_vertexBufferDirty)
		{
			if (m_vertices != nullptr)
			{
				D3D11VertexBuffer* d3dVertexBuffer = static_cast<D3D11VertexBuffer*>(const_cast<VertexBuffer*>(m_vertices)->GetPimpl());
				ID3D11Buffer* buffer = static_cast<ID3D11Buffer*>(d3dVertexBuffer->GetInternalBuffer());
				unsigned int stride = m_vertices->GetDeclaration()->GetStride();
				unsigned int offset = 0;
				m_deviceContext->IASetVertexBuffers(0, 1, &buffer, &stride, &offset); 
			}
			else
			{
				// TODO
			}
		}

		if (m_effectDirty)
		{
			m_effect->ApplyProgram(m_effectProgram);
		}

		if (m_effectDirty || m_vertexBufferDirty)
		{
			m_deviceContext->IASetInputLayout(getLayout(m_effect, m_effectProgram, *m_vertices->GetDeclaration()));
			m_vertexBufferDirty = false;
			m_effectDirty = false;
		}

		m_effect->SetConstantBuffers();

		SetSamplers();
	}

	ID3D11InputLayout* Direct3D11Device::getLayout(HlslEffect* shader, int program, const VertexDeclaration& decl)
	{
		uint64_t key = ((uint64_t)decl.GetHash() << 32) | (shader->GetHash(program));

		std::map<uint64_t, ID3D11InputLayout*>::iterator itr = m_layouts.find(key);
		if (itr == m_layouts.end())
		{
			static D3D11_INPUT_ELEMENT_DESC desc[16];
			const VertexElement* e = decl.GetElements();
			for (int i = 0; i < decl.GetNumElements(); i++)
			{
				if (e[i].ElementUsage == VertexElementUsage::Position)
					desc[i].SemanticName = "SV_POSITION";
				else if (e[i].ElementUsage == VertexElementUsage::Normal)
					desc[i].SemanticName = "NORMAL";
				else if (e[i].ElementUsage == VertexElementUsage::TextureCoordinate)
					desc[i].SemanticName = "TEXCOORD";
				else if (e[i].ElementUsage == VertexElementUsage::Color)
					desc[i].SemanticName = "COLOR";

				desc[i].SemanticIndex = e[i].UsageIndex;
			
				if (e[i].ElementFormat == VertexElementFormat::Single)
					desc[i].Format = DXGI_FORMAT_R32_FLOAT;
				else if (e[i].ElementFormat == VertexElementFormat::Vector2)
					desc[i].Format = DXGI_FORMAT_R32G32_FLOAT;
				else if (e[i].ElementFormat == VertexElementFormat::Vector3)
					desc[i].Format = DXGI_FORMAT_R32G32B32_FLOAT;
				else if (e[i].ElementFormat == VertexElementFormat::Vector4)
					desc[i].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
				else if (e[i].ElementFormat == VertexElementFormat::Color)
					desc[i].Format = DXGI_FORMAT_R8G8B8A8_UNORM;

				desc[i].InputSlot = 0;
				desc[i].AlignedByteOffset = e[i].Offset;
				desc[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
				desc[i].InstanceDataStepRate = 0;
			}

			byte* bytecode;
			int bytecodeLength;
			shader->GetBytecode(program, &bytecode, &bytecodeLength);
			ID3D11InputLayout* layout;
			if (FAILED(m_device->CreateInputLayout(desc, decl.GetNumElements(), bytecode, bytecodeLength, &layout)))
				throw GraphicsException("Unable to create vertex input layout", __FILE__, __LINE__);

			m_layouts.insert(std::map<uint64_t, ID3D11InputLayout*>::value_type(key, layout));

			return layout;
		}

		return (*itr).second;
	}
}
}
}

#endif
