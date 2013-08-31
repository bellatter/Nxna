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
		: VertexBuffer(device), m_declaration(*vertexDeclaration)
	{
		m_vertexCount = vertexCount;

		glGenBuffers(1, &m_buffer);

		GlException::ThrowIfError(__FILE__, __LINE__);
	}

	void GlVertexBuffer::SetData(int offsetInBytes, void* data, int numBytes)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_buffer);

		if (offsetInBytes == 0)
			glBufferData(GL_ARRAY_BUFFER, numBytes, data, GL_STATIC_DRAW);
		else
			glBufferSubData(GL_ARRAY_BUFFER, offsetInBytes, numBytes, data);

		GlException::ThrowIfError(__FILE__, __LINE__);
	}

	void GlVertexBuffer::Bind() const
	{
		GlException::ThrowIfError(__FILE__, __LINE__);
		glBindBuffer(GL_ARRAY_BUFFER, m_buffer);

		GlException::ThrowIfError(__FILE__, __LINE__);
	}

	GlDynamicVertexBuffer::GlDynamicVertexBuffer(OpenGlDevice* device, const VertexDeclaration* vertexDeclaration, int vertexCount, BufferUsage usage)
		: GlVertexBuffer(device, vertexDeclaration, vertexCount, usage)
		, DynamicVertexBuffer(device)
	{
	}

	void GlDynamicVertexBuffer::SetData(int offsetInBytes, void* data, int vertexCount)
	{
		m_vertexCount = vertexCount;
		int numBytes = m_declaration.GetStride() * vertexCount;

		glBindBuffer(GL_ARRAY_BUFFER, m_buffer);

		if (offsetInBytes == 0)
			glBufferData(GL_ARRAY_BUFFER, numBytes, data, GL_STATIC_DRAW);
		else
			glBufferSubData(GL_ARRAY_BUFFER, offsetInBytes, numBytes, data);

		GlException::ThrowIfError(__FILE__, __LINE__);
	}
}
}
}