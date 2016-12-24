#include <cassert>
#include "VertexBuffer.h"
#include "GraphicsDevice.h"
#include "VertexDeclaration.h"
#include "IVertexBufferPimpl.h"

namespace Nxna
{
namespace Graphics
{
	VertexBuffer::VertexBuffer(GraphicsDevice* device, const VertexDeclaration* vertexDeclaration, int vertexCount, BufferUsage usage)
		: m_declaration(*vertexDeclaration)
	{
		m_device = device;
		m_vertexCount = vertexCount;
		m_pimpl = device->CreateVertexBufferPimpl(false, vertexDeclaration, vertexCount, usage);
	}

	VertexBuffer::~VertexBuffer()
	{
		delete m_pimpl;
	}

	void VertexBuffer::SetData(int offsetInBytes, void* data, int numElements, int vertexStride, int elementSizeInBytes)
	{
		// TODO: do something with vertexStride

		SetData(offsetInBytes, data, numElements * elementSizeInBytes);
	}

	void VertexBuffer::SetData(void* data, int numVertices)
	{
		SetData(0, data, numVertices * GetDeclaration()->GetStride());
	}

	void VertexBuffer::SetData(int offsetInBytes, void* data, int numBytes)
	{
		m_pimpl->SetData(offsetInBytes, data, numBytes);
	}

	DynamicVertexBuffer::DynamicVertexBuffer(GraphicsDevice* device, const VertexDeclaration* vertexDeclaration, int vertexCount, BufferUsage usage)
		: VertexBuffer(vertexDeclaration)
	{
		m_device = device;
		m_vertexCount = vertexCount;
		m_pimpl = device->CreateVertexBufferPimpl(true, vertexDeclaration, vertexCount, usage);
	}

	DynamicVertexBuffer::~DynamicVertexBuffer()
	{
		// m_pimpl is deleted in the VertexBuffer destructor
	}
}
}