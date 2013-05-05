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

		virtual void SetData(void* data, int numVertices) = 0;
		virtual const VertexDeclaration* GetDeclaration() const = 0;

		virtual int GetVertexCount() const = 0;
	};

	class DynamicVertexBuffer : public VertexBuffer
	{
	public:

		DynamicVertexBuffer(GraphicsDevice* device)
			: VertexBuffer(device) {}
		virtual ~DynamicVertexBuffer() { }

		virtual void SetData(void* data, int numVertices) = 0;
		virtual const VertexDeclaration* GetDeclaration() const = 0;
	};
}
}

#endif // GRAPHICS_VERTEXBUFFER_H