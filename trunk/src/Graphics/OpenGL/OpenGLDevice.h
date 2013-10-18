#ifndef GRAPHICS_OPENGL_OPENGLDEVICE_H
#define GRAPHICS_OPENGL_OPENGLDEVICE_H

#include "../GraphicsDevice.h"
#include "../../Color.h"

NXNA_DISABLE_OVERRIDE_WARNING

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
		Viewport m_viewport;
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
		char m_vendor[128];
		char m_renderer[128];

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
		virtual void DrawUserPrimitives(PrimitiveType primitiveType, void* data, int primitiveCount, const VertexDeclaration* vertexDeclaration) override;

		virtual void SetVertexBuffer(const VertexBuffer* vertexBuffer) override;
		virtual void SetBlendState(const BlendState* blendState) override;

		virtual void Present() override;

		virtual BasicEffect* CreateBasicEffect() override;
		virtual SpriteEffect* CreateSpriteEffect() override;
		virtual DualTextureEffect* CreateDualTextureEffect() override;
		virtual AlphaTestEffect* CreateAlphaTestEffect() override;

		virtual void GetBackBufferData(void* data) override;

		void SetCurrentEffect(GlslEffect* effect) { m_effect = effect; }

		const char* GetVendor();
		const char* GetRenderer();
		int GetVersion() { return m_version; }
		int GetGlslVersion() { return m_glslVersion; }

	protected:
		virtual void SetSamplers() override;

		virtual Pvt::ITexture2DPimpl* CreateTexture2DPimpl(int width, int height, bool mipMap, SurfaceFormat format) override;
		virtual Pvt::IIndexBufferPimpl* CreateIndexBufferPimpl(IndexElementSize elementSize) override;
		virtual Pvt::IVertexBufferPimpl* CreateVertexBufferPimpl(bool dynamic, const VertexDeclaration* vertexDeclaration, int vertexCount, BufferUsage usage) override;

	private:
		void applyDirtyStates();

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

		GlException(int glError)
			: GraphicsException("GlException")
		{
			m_message = "GlException: err " + glError;
		}

		GlException(int glError, const char* file, int line);

#ifdef NXNA_DISABLE_OPENGL_ERRORS
		static void ThrowIfError(const char* /* filename */, int /* line */) { }
#else
		static void ThrowIfError(const char* filename, int line);
#endif
	};
}
}
}

NXNA_ENABLE_OVERRIDE_WARNING

#endif // GRAPHICS_OPENGL_OPENGLDEVICE_H
