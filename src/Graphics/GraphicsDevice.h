#ifndef GRAPHICS_GRAPHICSDEVICE_H
#define GRAPHICS_GRAPHICSDEVICE_H

#include "../Color.h"
#include "../Exception.h"
#include "BlendState.h"
#include "DepthStencilState.h"
#include "RasterizerState.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Texture2D.h"
#include "../Vector3.h"

namespace Nxna
{
namespace Graphics
{
	struct Viewport
	{
		Viewport()
		{
			X = 0;
			Y = 0;
			Width = 0;
			Height = 0;
		}

		Viewport(int x, int y, int width, int height)
		{
			X = x;
			Y = y;
			Width = width;
			Height = height;
		}

		float GetAspectRatio() const { return Width / (float)Height; }
		Nxna::Vector3 Unproject(const Nxna::Vector3& source, 
			const Nxna::Matrix& projection, 
			const Nxna::Matrix& view, 
			const Nxna::Matrix& world);

		int X, Y, Width, Height;
	};

	enum PrimitiveType
	{
		PrimitiveType_TriangleList
	};

	enum ClearOptions
	{
		ClearOptions_DepthBuffer = 0x01,
		ClearOptions_Stencil = 0x02,
		ClearOptions_Target = 0x04
	};

	class VertexDeclaration;
	class VertexBuffer;
	class IndexBuffer;
	class Texture2D;
	class BasicEffect;
	class SpriteEffect;
	class DualTextureEffect;
	class AlphaTestEffect;
	class IVertexBufferPimpl;
	class IDynamicVertexBufferPimpl;

	class GraphicsDevice
	{
	protected:
		static GraphicsDevice* m_instance;

	public:

		virtual ~GraphicsDevice() { }

		virtual CullMode GetRasterizerState() = 0;
		virtual void SetRasterizerState(const RasterizerState* state) = 0;

		virtual DepthStencilState GetDepthStencilState() = 0;
		virtual void SetDepthStencilState(const DepthStencilState* state) = 0;

		virtual void SetIndices(const IndexBuffer* indices) = 0;

		virtual void Clear(const Color& c) = 0;
		virtual void Clear(ClearOptions options, const Color& c, float depth, int stencil) = 0;
		virtual Viewport GetViewport() = 0;
		virtual void SetViewport(const Viewport& viewport) = 0;

		virtual void DrawIndexedPrimitives(PrimitiveType primitiveType, int baseVertex, int minVertexIndex, int numVertices, int startIndex, int primitiveCount) = 0;
		virtual void DrawPrimitives(PrimitiveType primitiveType, int startVertex, int primitiveCount) = 0;
		virtual void DrawUserIndexedPrimitives(PrimitiveType primitiveType, void* data, int numVertices, int* indices, int primitiveCount, const VertexDeclaration* vertexDeclaration) = 0;
		virtual void DrawUserIndexedPrimitives(PrimitiveType primitiveType, void* data, int numVertices, short* indices, int primitiveCount, const VertexDeclaration* vertexDeclaration) = 0;
		virtual void DrawUserPrimitives(PrimitiveType primitiveType, float* data, int primitiveCount, const VertexDeclaration* vertexDeclaration) = 0;

		virtual void SetVertexBuffer(const VertexBuffer* vertexBuffer) = 0;
		virtual void SetBlendState(const BlendState* blendState) = 0;

		virtual void Present() = 0;

		virtual Texture2D* CreateTexture(int width, int height) = 0;
		virtual Texture2D* CreateTexture(int width, int height, SurfaceFormat format) = 0;
		virtual BasicEffect* CreateBasicEffect() = 0;
		virtual SpriteEffect* CreateSpriteEffect() = 0;
		virtual DualTextureEffect* CreateDualTextureEffect() = 0;
		virtual AlphaTestEffect* CreateAlphaTextEffect() = 0;
		virtual VertexBuffer* CreateVertexBuffer(const VertexDeclaration* vertexDeclaration, int vertexCount, BufferUsage usage) = 0;
		virtual DynamicVertexBuffer* CreateDynamicVertexBuffer(const VertexDeclaration* vertexDeclaration, int vertexCount, BufferUsage usage) = 0;
		virtual IndexBuffer* CreateIndexBuffer(IndexElementSize elementSize) = 0;

		static GraphicsDevice* GetDevice() { return m_instance; }
	};

	class GraphicsException : public Exception
	{
	public:

		GraphicsException(const std::string& message)
			: Exception(message)
		{
		}
	};
}
}

#endif // GRAPHICS_GRAPHICSDEVICE_H