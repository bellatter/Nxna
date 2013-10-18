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
	GlVertexBuffer::GlVertexBuffer(bool dynamic, OpenGlDevice* device, const VertexDeclaration* vertexDeclaration, int vertexCount, BufferUsage usage)
		: m_declaration(*vertexDeclaration)
	{
		m_dynamic = dynamic;
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
}
}
}