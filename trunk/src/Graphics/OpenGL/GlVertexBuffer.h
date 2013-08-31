#ifndef GRAPHICS_OPENGL_GLVERTEXBUFFER_H
#define GRAPHICS_OPENGL_GLVERTEXBUFFER_H

#include "../VertexBuffer.h"
#include "../VertexDeclaration.h"

NXNA_DISABLE_OVERRIDE_WARNING

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
		VertexDeclaration m_declaration;
		int m_vertexCount;

	public:
		GlVertexBuffer(OpenGlDevice* device, const VertexDeclaration* vertexDeclaration, int vertexCount, BufferUsage usage);

		virtual const VertexDeclaration* GetDeclaration() const override { return &m_declaration; }
		virtual int GetVertexCount() const override { return m_vertexCount; }

		void Bind() const;

	protected:
		virtual void SetData(int offsetInBytes, void* data, int numBytes) override;
	};

	class GlDynamicVertexBuffer : public DynamicVertexBuffer, public GlVertexBuffer
	{
	public:
		GlDynamicVertexBuffer(OpenGlDevice* device, const VertexDeclaration* vertexDeclaration, int vertexCount, BufferUsage usage);

		virtual void SetData(int offsetInBytes, void* data, int numVertices) override;
		virtual const VertexDeclaration* GetDeclaration() const override { return &m_declaration; }
		virtual int GetVertexCount() const override { return m_vertexCount; }
	};
}
}
}

NXNA_ENABLE_OVERRIDE_WARNING

#endif // GRAPHICS_OPENGL_GLVERTEXBUFFER_H