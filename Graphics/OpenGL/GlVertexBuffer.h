#ifndef GRAPHICS_OPENGL_GLVERTEXBUFFER_H
#define GRAPHICS_OPENGL_GLVERTEXBUFFER_H

#include "../VertexBuffer.h"

namespace Nxna
{
namespace Graphics
{
namespace OpenGl
{
	class GlVertexBuffer : public VertexBuffer
	{
	protected:
		unsigned int m_buffer;
		const VertexDeclaration* m_declaration;
		int m_vertexCount;

	public:
		GlVertexBuffer(OpenGlDevice* device, const VertexDeclaration* vertexDeclaration, int vertexCount, BufferUsage usage);

		virtual void SetData(void* data, int numVertices) override;
		virtual const VertexDeclaration* GetDeclaration() const override { return m_declaration; }
		virtual int GetVertexCount() const override { return m_vertexCount; }

		void Bind() const;
	};

	class GlDynamicVertexBuffer : public DynamicVertexBuffer, public GlVertexBuffer
	{
	public:
		GlDynamicVertexBuffer(OpenGlDevice* device, const VertexDeclaration* vertexDeclaration, int vertexCount, BufferUsage usage);

		virtual void SetData(void* data, int numVertices) override;
		virtual const VertexDeclaration* GetDeclaration() const override { return m_declaration; }
		virtual int GetVertexCount() const override { return m_vertexCount; }
	};
}
}
}

#endif // GRAPHICS_OPENGL_GLVERTEXBUFFER_H