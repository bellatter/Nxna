#ifndef NXNA_GRAPHICS_DIRECT3D_D3D11VERTEXBUFFER_H
#define NXNA_GRAPHICS_DIRECT3D_D3D11VERTEXBUFFER_H

#include "../VertexBuffer.h"
#include "../IVertexBufferPimpl.h"
#include "../VertexDeclaration.h"
#include "../../NxnaConfig.h"

NXNA_DISABLE_OVERRIDE_WARNING

namespace Nxna
{
namespace Graphics
{
namespace Direct3D11
{
	class D3D11VertexBuffer : public Pvt::IVertexBufferPimpl
	{
		Direct3D11Device* m_device;
		VertexDeclaration m_declaration;
		int m_vertexCount;
		void* m_buffer;

	public:
		D3D11VertexBuffer(bool dynamic, Direct3D11Device* device, const VertexDeclaration* vertexDeclaration, int vertexCount, BufferUsage usage);

		virtual void SetData(int offsetInBytes, void* data, int numBytes) override;


		void* GetInternalBuffer() const { return m_buffer; }

	private:
		void setDataStatic(int offsetInBytes, void* data, int numBytes);
		void setDataDynamic(int offsetInBytes, void* data, int numBytes);
	};
}
}
}

NXNA_ENABLE_OVERRIDE_WARNING

#endif // NXNA_GRAPHICS_DIRECT3D_D3D11VERTEXBUFFER_H