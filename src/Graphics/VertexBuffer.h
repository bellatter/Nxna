#ifndef GRAPHICS_VERTEXBUFFER_H
#define GRAPHICS_VERTEXBUFFER_H

#include "../NxnaConfig.h"
#include "VertexDeclaration.h"

namespace Nxna
{
namespace Graphics
{
	class GraphicsDevice;

	namespace Pvt
	{
		class IVertexBufferPimpl;
	}

	NXNA_ENUM(BufferUsage)
		None,
		WriteOnly
	END_NXNA_ENUM(BufferUsage)

	class VertexBuffer
	{
	protected:
		GraphicsDevice* m_device;
		VertexDeclaration m_declaration;
		int m_vertexCount;
		Pvt::IVertexBufferPimpl* m_pimpl;

	public:

		VertexBuffer(GraphicsDevice* device, const VertexDeclaration* vertexDeclaration, int vertexCount, BufferUsage usage);
		virtual ~VertexBuffer();
		GraphicsDevice* GetGraphicsDevice() { return m_device; }

		void SetData(int offsetInBytes, void* data, int numElements, int vertexStride, int elementSizeInBytes);
		void SetData(void* data, int numVertices);

		const VertexDeclaration* GetDeclaration() const { return &m_declaration; }

		int GetVertexCount() const { return m_vertexCount; }

		Pvt::IVertexBufferPimpl* GetPimpl() { return m_pimpl; }

	protected:
		VertexBuffer(const VertexDeclaration* vertexDeclaration)
		: m_declaration(*vertexDeclaration)
		{ 
			// this is just used by DynamicVertexBuffer
		} 

		void SetData(int offsetInBytes, void* data, int numBytes);
	};

	class DynamicVertexBuffer : public VertexBuffer
	{
	public:

		DynamicVertexBuffer(GraphicsDevice* device, const VertexDeclaration* vertexDeclaration, int vertexCount, BufferUsage usage);
		virtual ~DynamicVertexBuffer();
	};
}
}

#endif // GRAPHICS_VERTEXBUFFER_H