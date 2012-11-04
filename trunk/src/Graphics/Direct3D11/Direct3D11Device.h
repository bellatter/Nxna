#ifndef GRAPHICS_DIRECT3D11_DIRECT3D11DEVICE_H
#define GRAPHICS_DIRECT3D11_DIRECT3D11DEVICE_H

#include "../GraphicsDevice.h"
#include "../../Color.h"

struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;

NXNA_DISABLE_OVERRIDE_WARNING

namespace Nxna
{
namespace Graphics
{
namespace Direct3D11
{
	class Direct3D11Device : public GraphicsDevice
	{
		ID3D11Device* m_device;
		ID3D11DeviceContext* m_deviceContext;
		IDXGISwapChain* m_swapChain;

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

		virtual Texture2D* CreateTexture(int width, int height) override;
		virtual Texture2D* CreateTexture(int width, int height, SurfaceFormat format) override;
		virtual BasicEffect* CreateBasicEffect() override;
		virtual SpriteEffect* CreateSpriteEffect() override;
		virtual DualTextureEffect* CreateDualTextureEffect() override;
		virtual AlphaTestEffect* CreateAlphaTestEffect() override;
		virtual VertexBuffer* CreateVertexBuffer(const VertexDeclaration* vertexDeclaration, int vertexCount, BufferUsage usage) override;
		virtual DynamicVertexBuffer* CreateDynamicVertexBuffer(const VertexDeclaration* vertexDeclaration, int vertexCount, BufferUsage usage) override;
		virtual IndexBuffer* CreateIndexBuffer(IndexElementSize elementSize) override;

		virtual void GetBackBufferData(void* data) override;

		void* GetDevice() { return m_device; }
		void* GetDeviceContext() { return m_deviceContext; }
	};

}
}
}

NXNA_ENABLE_OVERRIDE_WARNING

#endif // GRAPHICS_DIRECT3D11_DIRECT3D11DEVICE_H
