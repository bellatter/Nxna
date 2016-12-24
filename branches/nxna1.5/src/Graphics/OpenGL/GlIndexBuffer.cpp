#include "OpenGL.h"
#include "GlIndexBuffer.h"
#include <cassert>

namespace Nxna
{
namespace Graphics
{
namespace OpenGl
{
	GlIndexBuffer::GlIndexBuffer(IndexElementSize indexElementSize)
		: Pvt::IIndexBufferPimpl(indexElementSize)
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

	void GlIndexBuffer::SetData(int offsetInBytes, void* data, int indexCount)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffer);

		if (offsetInBytes == 0)
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * (int)m_elementSize, data, GL_STATIC_DRAW);
		else
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offsetInBytes, indexCount * (int)m_elementSize, data);

		int totalIndexCount = indexCount + offsetInBytes / (int)m_elementSize;

#ifndef NDEBUG
		if (m_indices == nullptr)
			m_indices = new int[totalIndexCount];
		else
			assert(totalIndexCount <= m_indexCount);

		for (int i = 0; i < indexCount; i++)
		{
			if (m_elementSize == IndexElementSize::SixteenBits)
				m_indices[i + offsetInBytes / 2] = ((short*)data)[i];
			else
				m_indices[i + offsetInBytes / 4] = ((int*)data)[i];
		}
#endif

		m_indexCount = totalIndexCount;
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
