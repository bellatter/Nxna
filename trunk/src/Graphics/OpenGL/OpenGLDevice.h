#ifndef GRAPHICS_OPENGL_OPENGLDEVICE_H
#define GRAPHICS_OPENGL_OPENGLDEVICE_H

#include "../GraphicsDevice.h"
#include "../../Color.h"

namespace Nxna
{
namespace Graphics
{
namespace OpenGl
{
	class GlslEffect;
	class GlIndexBuffer;

	class OpenGlDevice : public GraphicsDevice
	{
		Color m_clearColor;
		float m_clearDepth;
		int m_clearStencil;
		bool m_vertexPointersNeedSetup;
		const VertexDeclaration* m_declaration;
		GlslEffect* m_effect;
		const VertexBuffer* m_vertices;
		const GlIndexBuffer* m_indices;
		int m_version;
		int m_glslVersion;

		int m_screenWidth, m_screenHeight;

	public:
		OpenGlDevice();
		void OnContextCreated();
		void UpdateScreenSize(int screenWidth, int screenHeight);

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
		virtual void DrawUserPrimitives(PrimitiveType primitiveType, float* data, int primitiveCount, const VertexDeclaration* vertexDeclaration) override;

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

		void SetCurrentEffect(GlslEffect* effect) { m_effect = effect; }

		int GetVersion() { return m_version; }
		int GetGlslVersion() { return m_glslVersion; }

	private:

		void setClearColor(const Color& c);
		void setClearDepth(float depth);
		void setClearStencil(int s);
		void setupVertexBufferPointers(void* verts);
		
		static int convertCompareFunction(CompareFunction func);
		static CompareFunction convertCompareFunction(int func);
		static int convertStencilOperation(StencilOperation op);
		static StencilOperation convertStencilOperation(int op);
		static int convertBlendMode(Blend mode);
		static int convertBlendFunc(BlendFunction func);
	};

	class GlException : public GraphicsException
	{
	public:

		GlException()
			: GraphicsException("GlException")
		{ }

		static void ThrowIfError();
	};
}
}
}

#endif // GRAPHICS_OPENGL_OPENGLDEVICE_H
