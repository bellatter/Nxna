#include <cassert>
#include "OpenGL.h"
#include "OpenGLDevice.h"
#include "GlVertexBuffer.h"
#include "../VertexDeclaration.h"

namespace Nxna
{
namespace Graphics
{
namespace OpenGl
{
	GlVertexBuffer::GlVertexBuffer(OpenGlDevice* device, const VertexDeclaration* vertexDeclaration, int vertexCount, BufferUsage usage)
		: VertexBuffer(device)
	{
		m_declaration = vertexDeclaration;
		m_vertexCount = vertexCount;

		glGenBuffers(1, &m_buffer);

		GlException::ThrowIfError();
	}

	void GlVertexBuffer::SetData(void* data, int vertexCount)
	{
		assert(vertexCount == m_vertexCount);

		int numBytes = m_declaration->GetStride() * m_vertexCount;

		glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
		glBufferData(GL_ARRAY_BUFFER, numBytes, data, GL_STATIC_DRAW);

		GlException::ThrowIfError();
	}

	void GlVertexBuffer::Bind() const
	{
		GlException::ThrowIfError();
		glBindBuffer(GL_ARRAY_BUFFER, m_buffer);

		GlException::ThrowIfError();
	}

	GlDynamicVertexBuffer::GlDynamicVertexBuffer(OpenGlDevice* device, const VertexDeclaration* vertexDeclaration, int vertexCount, BufferUsage usage)
		: GlVertexBuffer(device, vertexDeclaration, vertexCount, usage)
		, DynamicVertexBuffer(device)
	{
	}

	void GlDynamicVertexBuffer::SetData(void* data, int vertexCount)
	{
		m_vertexCount = vertexCount;
		int numBytes = m_declaration->GetStride() * vertexCount;

		glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
		glBufferData(GL_ARRAY_BUFFER, numBytes, data, GL_STATIC_DRAW);

		GlException::ThrowIfError();
	}
}
}
}