#include "Direct3D11Device.h"
#include "D3D11Texture2D.h"
#include "D3D11VertexBuffer.h"
#include "D3D11IndexBuffer.h"
#include "HlslSpriteEffect.h"
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

		m_indices = nullptr;
		m_vertices = nullptr;
		m_indexBufferDirty = true;
		m_vertexBufferDirty = true;
		m_blendStateDirty = true;
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
		return CullMode::CullMode_CullClockwiseFace;
	}

	void Direct3D11Device::SetRasterizerState(const RasterizerState* state) {}

	DepthStencilState Direct3D11Device::GetDepthStencilState()
	{
		return *DepthStencilState::GetDefault();
	}

	void Direct3D11Device::SetDepthStencilState(const DepthStencilState* state)
	{
		m_depthState = *state;
		m_depthStencilStateDirty = true;
	}

	void Direct3D11Device::SetIndices(const IndexBuffer* indices)
	{
		m_indices = static_cast<const D3D11IndexBuffer*>(indices);
		m_indexBufferDirty = true;
	}

	void Direct3D11Device::Clear(const Color& c)
	{
		float color[4];
		color[0] = c.R / 255.0f;
		color[1] = c.G / 255.0f;
		color[2] = c.B / 255.0f;
		color[3] = c.A / 255.0f;

		m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);
	}

	void Direct3D11Device::Clear(ClearOptions options, const Color& c, float depth, int stencil) { }

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
		if (primitiveType == PrimitiveType_TriangleList)
		{
			indexCount = primitiveCount * 3;
			m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		}
		else if (primitiveType == PrimitiveType_TriangleStrip)
		{
			indexCount = primitiveCount * 3; // FIXME
			m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		}

		m_deviceContext->DrawIndexed(indexCount, startIndex, baseVertex);
	}

	void Direct3D11Device::DrawPrimitives(PrimitiveType primitiveType, int startVertex, int primitiveCount) { }
	void Direct3D11Device::DrawUserIndexedPrimitives(PrimitiveType primitiveType, void* data, int numVertices, int* indices, int primitiveCount, const VertexDeclaration* vertexDeclaration) { }
	void Direct3D11Device::DrawUserIndexedPrimitives(PrimitiveType primitiveType, void* data, int numVertices, short* indices, int primitiveCount, const VertexDeclaration* vertexDeclaration) { }
	void Direct3D11Device::DrawUserPrimitives(PrimitiveType primitiveType, void* data, int primitiveCount, const VertexDeclaration* vertexDeclaration) { }

	void Direct3D11Device::SetVertexBuffer(const VertexBuffer* vertexBuffer)
	{
		m_vertices = dynamic_cast<const D3D11VertexBuffer*>(vertexBuffer); // static_cast<> won't work due to multiple inheritance :(
		m_vertexBufferDirty = true;
	}

	void Direct3D11Device::SetBlendState(const BlendState* blendState)
	{
		m_blendState = *blendState;
		m_blendStateDirty = true;
	}

	void Direct3D11Device::Present()
	{
		m_swapChain->Present(0, 0);
	}

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

	unsigned int Direct3D11Device::CalcShaderHash(const byte* bytecode, int length)
	{
		return Utils::CalcHash(bytecode, length);
	}

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
				blendState.RenderTarget[0].BlendEnable = false;
				blendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
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
				depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

				depthStencilDesc.StencilEnable = m_depthState.StencilEnable;
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

				if (FAILED(m_device->CreateDepthStencilState(&depthStencilDesc, (ID3D11DepthStencilState**)internalHandle)))
					throw GraphicsException("Unable to create D3D11 depth/stencil state", __FILE__, __LINE__);
			}
			
			m_deviceContext->OMSetDepthStencilState((ID3D11DepthStencilState*)*internalHandle, 1);

			m_depthStencilStateDirty = false;
		}

		if (m_indexBufferDirty)
		{
			if (m_indices != nullptr)
			{
				IndexElementSize elementSize = m_indices->GetElementSize();
				DXGI_FORMAT format;
				if (elementSize == IndexElementSize_SixteenBits)
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
				ID3D11Buffer* buffer = static_cast<ID3D11Buffer*>(m_vertices->GetInternalBuffer());
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
	}

	ID3D11InputLayout* Direct3D11Device::getLayout(HlslEffect* shader, int program, const VertexDeclaration& decl)
	{
		uint64_t key = (decl.GetHash() << 32) | (shader->GetHash(program));

		std::map<uint64_t, ID3D11InputLayout*>::iterator itr = m_layouts.find(key);
		if (itr == m_layouts.end())
		{
			static D3D11_INPUT_ELEMENT_DESC desc[16];
			const VertexElement* e = decl.GetElements();
			for (int i = 0; i < decl.GetNumElements(); i++)
			{
				if (e[i].ElementUsage == VEU_Position)
					desc[i].SemanticName = "SV_POSITION";
				else if (e[i].ElementUsage == VEU_Normal)
					desc[i].SemanticName = "NORMAL";
				else if (e[i].ElementUsage == VEU_TextureCoordinate)
					desc[i].SemanticName = "TEXCOORD";
				else if (e[i].ElementUsage == VEU_Color)
					desc[i].SemanticName = "COLOR";

				desc[i].SemanticIndex = e[i].UsageIndex;
			
				if (e[i].ElementFormat == VEF_Single)
					desc[i].Format = DXGI_FORMAT_R32_FLOAT;
				else if (e[i].ElementFormat == VEF_Vector2)
					desc[i].Format = DXGI_FORMAT_R32G32_FLOAT;
				else if (e[i].ElementFormat == VEF_Vector3)
					desc[i].Format = DXGI_FORMAT_R32G32B32_FLOAT;
				else if (e[i].ElementFormat == VEF_Vector4)
					desc[i].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
				else if (e[i].ElementFormat == VEF_Color)
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