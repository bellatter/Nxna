#ifndef GRAPHICS_VERTEXBUFFER_H
#define GRAPHICS_VERTEXBUFFER_H

namespace Nxna
{
namespace Graphics
{
	class GraphicsDevice;
	class VertexDeclaration;

	enum BufferUsage
	{
		BufferUsage_None,
		BufferUsage_WriteOnly
	};

	class VertexBuffer
	{
		GraphicsDevice* m_device;

	public:

		VertexBuffer(GraphicsDevice* device) { m_device = device; }
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

		virtual void SetData(void* data, int numVertices) = 0;
		virtual const VertexDeclaration* GetDeclaration() const = 0;
	};
}
}

#endif // GRAPHICS_VERTEXBUFFER_H