#include "IndexBuffer.h"
#include "IIndexBufferPimpl.h"
#include "GraphicsDevice.h"

namespace Nxna
{
namespace Graphics
{
	IndexBuffer::IndexBuffer(GraphicsDevice* device, IndexElementSize indexElementSize)
	{
		m_pimpl = device->CreateIndexBufferPimpl(indexElementSize);
		m_elementSize = indexElementSize;
	}

	IndexBuffer::~IndexBuffer()
	{
		delete m_pimpl;
	}

	void IndexBuffer::SetData(void* indices, int indexCount)
	{
		m_indexCount = indexCount;
		m_pimpl->SetData(0, indices, indexCount);
	}

	void IndexBuffer::SetData(int offsetInBytes, void* indices, int indexCount)
	{
		m_indexCount = indexCount;
		m_pimpl->SetData(offsetInBytes, indices, indexCount);
	}
}
}