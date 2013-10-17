#ifndef GRAPHICS_DIRECT3D11_DIRECT3D11DEVICE_H
#define GRAPHICS_DIRECT3D11_DIRECT3D11DEVICE_H

#include "../GraphicsDevice.h"
#include "../../Color.h"
#include <map>
#include <cstdint>

struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
struct ID3D11Texture2D;
struct ID3D11InputLayout;


NXNA_DISABLE_OVERRIDE_WARNING

namespace Nxna
{
namespace Graphics
{
namespace Direct3D11
{
	class D3D11VertexBuffer;
	class D3D11IndexBuffer;
	class HlslEffect;

	class Direct3D11Device : public GraphicsDevice
	{
		ID3D11Device* m_device;
		ID3D11DeviceContext* m_deviceContext;
		IDXGISwapChain* m_swapChain;
		ID3D11RenderTargetView* m_renderTargetView;
		ID3D11Texture2D* m_depthStencilBuffer;
		ID3D11DepthStencilView* m_depthStencilView;

		const D3D11VertexBuffer* m_vertices;
		const D3D11IndexBuffer* m_indices;
		HlslEffect* m_effect;
		int m_effectProgram;

		bool m_indexBufferDirty;
		bool m_vertexBufferDirty;
		bool m_effectDirty;
		bool m_blendStateDirty;
		bool m_depthStencilStateDirty;
		bool m_rasterizerStateDirty;

		BlendState m_blendState;
		DepthStencilState m_depthState;
		RasterizerState m_rasterizerState;

		std::map<uint64_t, ID3D11InputLayout*> m_layouts;

	public:
		Direct3D11Device();
		void OnWindowCreated(void* window, int width, int height);

		virtual CullMode GetRasterizerState() override;
		virtual void SetRasterizerState(const RasterizerState* state) override;

		virtual DepthStencilState GetDepthStencilState() override;
		virtual void SetDepthStencilState(const DepthStencilState* state) override;

		virtual void SetIndices(const IndexBuffer* indices) override;

		virtual void Clear(const Color& c) override;
		virtual void Clear(ClearOptions options, const Color& c, float depth, int stencil) override;
		virtual Viewport GetViewport() override;
		virtual void SetViewport(const Viewport& viewport) override;

		virtual void DrawIndexedPrimitives(PrimitiveType primitiveType, int baseVertex, int minVertexIndex, int numVertices, int startIndex, int primitiveCount) override;
		virtual void DrawPrimitives(PrimitiveType primitiveType, int startVertex, int primitiveCount) override;
		virtual void DrawUserIndexedPrimitives(PrimitiveType primitiveType, void* data, int numVertices, int* indices, int primitiveCount, const VertexDeclaration* vertexDeclaration) override;
		virtual void DrawUserIndexedPrimitives(PrimitiveType primitiveType, void* data, int numVertices, short* indices, int primitiveCount, const VertexDeclaration* vertexDeclaration) override;
		virtual void DrawUserPrimitives(PrimitiveType primitiveType, void* data, int primitiveCount, const VertexDeclaration* vertexDeclaration) override;

		virtual void SetVertexBuffer(const VertexBuffer* vertexBuffer) override;
		virtual void SetBlendState(const BlendState* blendState) override;

		virtual void Present() override;

		virtual BasicEffect* CreateBasicEffect() override;
		virtual SpriteEffect* CreateSpriteEffect() override;
		virtual DualTextureEffect* CreateDualTextureEffect() override;
		virtual AlphaTestEffect* CreateAlphaTestEffect() override;
		virtual VertexBuffer* CreateVertexBuffer(const VertexDeclaration* vertexDeclaration, int vertexCount, BufferUsage usage) override;
		virtual DynamicVertexBuffer* CreateDynamicVertexBuffer(const VertexDeclaration* vertexDeclaration, int vertexCount, BufferUsage usage) override;

		virtual void GetBackBufferData(void* data) override;

		void* GetDevice() { return m_device; }
		void* GetDeviceContext() { return m_deviceContext; }

		HlslEffect* GetCurrentEffect() { return m_effect; }
		int GetCurrentProgramIndex() { return m_effectProgram; }

		void SetCurrentEffect(HlslEffect* effect, int program) { m_effect = effect; m_effectProgram = program; m_effectDirty = true; }
		unsigned int CalcShaderHash(const byte* bytecode, int length);

	protected:
		virtual void SetSamplers() override;
		virtual Pvt::ITexture2DPimpl* CreateTexture2DPimpl(int width, int height, bool mipMap, SurfaceFormat format) override;
		virtual Pvt::IIndexBufferPimpl* CreateIndexBufferPimpl(IndexElementSize elementSize) override;

	private:
		void applyDirtyStates();
		unsigned int hashVertexDeclaration(const VertexDeclaration& decl);
		ID3D11InputLayout* getLayout(HlslEffect* shader, int program, const VertexDeclaration& decl);
	};

}
}
}

NXNA_ENABLE_OVERRIDE_WARNING

#endif // GRAPHICS_DIRECT3D11_DIRECT3D11DEVICE_H
