#include "OpenGL.h"
#include "GlIndexBuffer.h"

namespace Nxna
{
namespace Graphics
{
namespace OpenGl
{
	GlIndexBuffer::GlIndexBuffer(IndexElementSize indexElementSize)
	{
		m_elementSize = indexElementSize;
		glGenBuffers(1, &m_buffer);

#ifndef NDEBUG
		m_indices = nullptr;
#endif
	}

	GlIndexBuffer::~GlIndexBuffer()
	{
		glDeleteBuffers(1, &m_buffer);
		
#ifndef NDEBUG
		delete[] m_indices;
#endif
	}

	void GlIndexBuffer::SetData(void* data, int indexCount)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * (int)m_elementSize, data, GL_STATIC_DRAW);

		m_indexCount = indexCount;

#ifndef NDEBUG
		m_indices = new int[indexCount];
		for (int i = 0; i < indexCount; i++)
		{
			if (m_elementSize == IndexElementSize::SixteenBits)
				m_indices[i] = ((short*)data)[i];
			else
				m_indices[i] = ((int*)data)[i];
		}
#endif
	}

	void GlIndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffer);
	}

#ifndef NDEBUG
	int GlIndexBuffer::GetIndex(int index) const
	{
		return m_indices[index];
	}
#endif
}
}
}
