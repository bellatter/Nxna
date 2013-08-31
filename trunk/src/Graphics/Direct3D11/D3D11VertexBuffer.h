#ifndef NXNA_GRAPHICS_DIRECT3D_D3D11VERTEXBUFFER_H
#define NXNA_GRAPHICS_DIRECT3D_D3D11VERTEXBUFFER_H

#include "../VertexBuffer.h"
#include "../../NxnaConfig.h"

NXNA_DISABLE_OVERRIDE_WARNING

namespace Nxna
{
namespace Graphics
{
namespace Direct3D11
{
	class D3D11VertexBuffer : public VertexBuffer
	{
	protected:
		void* m_buffer;
		const VertexDeclaration* m_declaration;
		int m_vertexCount;

	public:
		D3D11VertexBuffer(Direct3D11Device* device, const VertexDeclaration* vertexDeclaration, int vertexCount, BufferUsage usage);

		virtual void SetData(int offsetInBytes, void* data, int numVertices) override;
		virtual const VertexDeclaration* GetDeclaration() const override { return m_declaration; }
		virtual int GetVertexCount() const override { return m_vertexCount; }

		void* GetInternalBuffer() const { return m_buffer; }
	};

	class D3D11DynamicVertexBuffer : public DynamicVertexBuffer, public D3D11VertexBuffer
	{
	public:
		D3D11DynamicVertexBuffer(Direct3D11Device* device, const VertexDeclaration* vertexDeclaration, int vertexCount, BufferUsage usage);

		virtual void SetData(int offsetInBytes, void* data, int numVertices) override;
		virtual const VertexDeclaration* GetDeclaration() const override { return m_declaration; }
		virtual int GetVertexCount() const override { return m_vertexCount; }
	};
}
}
}

NXNA_ENABLE_OVERRIDE_WARNING

#endif // NXNA_GRAPHICS_DIRECT3D_D3D11VERTEXBUFFER_H