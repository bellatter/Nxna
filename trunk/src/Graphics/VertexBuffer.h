#ifndef GRAPHICS_VERTEXBUFFER_H
#define GRAPHICS_VERTEXBUFFER_H

#include "../NxnaConfig.h"

namespace Nxna
{
namespace Graphics
{
	class GraphicsDevice;
	class VertexDeclaration;

	NXNA_ENUM(BufferUsage)
		None,
		WriteOnly
	END_NXNA_ENUM(BufferUsage)

	class VertexBuffer
	{
		GraphicsDevice* m_device;

	public:

		VertexBuffer(GraphicsDevice* device) { m_device = device; }
		virtual ~VertexBuffer() { }
		GraphicsDevice* GetGraphicsDevice() { return m_device; }

		void SetData(int offsetInBytes, void* data, int numElements, int vertexStride, int elementSizeInBytes);
		void SetData(void* data, int numVertices);

		virtual const VertexDeclaration* GetDeclaration() const = 0;

		virtual int GetVertexCount() const = 0;

	protected:
		virtual void SetData(int offsetInBytes, void* data, int numBytes) = 0;
	};

	class DynamicVertexBuffer : public VertexBuffer
	{
	public:

		DynamicVertexBuffer(GraphicsDevice* device)
			: VertexBuffer(device) {}
		virtual ~DynamicVertexBuffer() { }

		virtual void SetData(int offsetInBytes, void* data, int numVertices) = 0;
		void SetData(void* data, int numVertices)
		{
			SetData(0, data, numVertices);
		}

		virtual const VertexDeclaration* GetDeclaration() const = 0;
	};
}
}

#endif // GRAPHICS_VERTEXBUFFER_H