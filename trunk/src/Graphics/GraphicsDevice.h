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
#include "SamplerStateCollection.h"
#include "../Vector3.h"
#include "../Rectangle.h"

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

		Nxna::Vector3 Project(const Nxna::Vector3& source,
			const Nxna::Matrix& project,
			const Nxna::Matrix& view,
			const Nxna::Matrix& world);

		Nxna::Vector3 Unproject(const Nxna::Vector3& source, 
			const Nxna::Matrix& projection, 
			const Nxna::Matrix& view, 
			const Nxna::Matrix& world);

		Rectangle GetBounds()
		{
			return Rectangle(X, Y, Width, Height);
		}

		int X, Y, Width, Height;
	};

	NXNA_ENUM(PrimitiveType)
		TriangleList,
		TriangleStrip
	END_NXNA_ENUM(PrimitiveType)

	NXNA_ENUM(ClearOptions)
		DepthBuffer = 0x01,
		Stencil = 0x02,
		Target = 0x04
	END_NXNA_ENUM(ClearOptions)

#ifdef NXNA_CLASS_ENUMS_NOT_SUPPORTED
	inline ClearOptions operator|(ClearOptions& a, ClearOptions& b)
#else
	inline ClearOptions operator|(ClearOptions a, ClearOptions b)
#endif
	{
		return static_cast<ClearOptions>(static_cast<int>(a) | static_cast<int>(b));
	}

#ifdef NXNA_CLASS_ENUMS_NOT_SUPPORTED
	inline ClearOptions operator&(ClearOptions& a, ClearOptions& b)
#else
	inline ClearOptions operator&(ClearOptions a, ClearOptions b)
#endif
	{
		return static_cast<ClearOptions>(static_cast<int>(a) & static_cast<int>(b));
	}

	class VertexDeclaration;
	class VertexBuffer;
	class IndexBuffer;
	class Texture2D;
	class BasicEffect;
	class SpriteEffect;
	class DualTextureEffect;
	class AlphaTestEffect;
	struct GraphicsDeviceCapabilities;
	class IVertexBufferPimpl;
	class IDynamicVertexBufferPimpl;

	namespace Pvt
	{
		class ITexture2DPimpl;
		class IIndexBufferPimpl;
	}

	class GraphicsDevice
	{
		friend class Texture2D;
		friend class IndexBuffer;

	protected:
		static GraphicsDevice* m_instance;
		GraphicsDeviceCapabilities* m_caps;
		SamplerStateCollection m_samplers;

	public:
		virtual ~GraphicsDevice();

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
		virtual void DrawUserPrimitives(PrimitiveType primitiveType, void* data, int primitiveCount, const VertexDeclaration* vertexDeclaration) = 0;

		virtual void SetVertexBuffer(const VertexBuffer* vertexBuffer) = 0;
		virtual void SetBlendState(const BlendState* blendState) = 0;

		virtual void Present() = 0;

		virtual BasicEffect* CreateBasicEffect() = 0;
		virtual SpriteEffect* CreateSpriteEffect() = 0;
		virtual DualTextureEffect* CreateDualTextureEffect() = 0;
		virtual AlphaTestEffect* CreateAlphaTestEffect() = 0;
		virtual VertexBuffer* CreateVertexBuffer(const VertexDeclaration* vertexDeclaration, int vertexCount, BufferUsage usage) = 0;
		virtual DynamicVertexBuffer* CreateDynamicVertexBuffer(const VertexDeclaration* vertexDeclaration, int vertexCount, BufferUsage usage) = 0;

		SamplerStateCollection& GetSamplerStates() { return m_samplers; }

		virtual void GetBackBufferData(void* data) = 0;

		GraphicsDeviceCapabilities* GetCaps() { return m_caps; }

		static GraphicsDevice* GetDevice() { return m_instance; }

	protected:
		void** GetInternalHandle(BlendState* blendState) { return &blendState->m_handle; }
		void** GetInternalHandle(DepthStencilState* depthStencilState) { return &depthStencilState->m_handle; }
		void *const* GetInternalHandle(const SamplerState* samplerState) { return &samplerState->m_handle; }
		void** GetInternalHandle(RasterizerState* rasterizerState) { return &rasterizerState->m_handle; }

		virtual void SetSamplers() = 0;

		virtual Pvt::ITexture2DPimpl* CreateTexture2DPimpl(int width, int height, bool mipMap, SurfaceFormat format) = 0;
		virtual Pvt::IIndexBufferPimpl* CreateIndexBufferPimpl(IndexElementSize elementSize) = 0;
	};

	class GraphicsException : public Exception
	{
	public:

		GraphicsException(const std::string& message)
			: Exception(message)
		{
		}

		GraphicsException(const std::string& message, const char* file, int line)
			: Exception(message, file, line)
		{
		}
	};
}
}

#endif // GRAPHICS_GRAPHICSDEVICE_H