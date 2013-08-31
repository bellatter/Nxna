#include <cassert>
#include "VertexBuffer.h"
#include "GraphicsDevice.h"
#include "VertexDeclaration.h"

namespace Nxna
{
namespace Graphics
{
	void VertexBuffer::SetData(int offsetInBytes, void* data, int numElements, int vertexStride, int elementSizeInBytes)
	{
		// TODO: do something with vertexStride

		SetData(offsetInBytes, data, numElements * elementSizeInBytes);
	}

	void VertexBuffer::SetData(void* data, int numVertices)
	{
		SetData(0, data, numVertices * GetDeclaration()->GetStride());
	}
}
}