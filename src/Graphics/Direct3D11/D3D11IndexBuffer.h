#ifndef NXNA_GRAPHICS_DIRECT3D11_D3D11INDEXBUFFER_H
#define NXNA_GRAPHICS_DIRECT3D11_D3D11INDEXBUFFER_H

#include "../../NxnaConfig.h"
#include "../IndexBuffer.h"
#include "../IIndexBufferPimpl.h"

NXNA_DISABLE_OVERRIDE_WARNING

namespace Nxna
{
namespace Graphics
{
namespace Direct3D11
{
	class D3D11IndexBuffer : public Pvt::IIndexBufferPimpl
	{
		Direct3D11Device* m_d3d11Device;
		void* m_indexBuffer;

	public:
		D3D11IndexBuffer(Direct3D11Device* d3d11Device, IndexElementSize elementSize);
		virtual ~D3D11IndexBuffer();

		virtual void SetData(int offsetInBytes, void* data, int indexCount) override;

		void* GetInternalBuffer() const { return m_indexBuffer; };
	};
}
}
}

NXNA_ENABLE_OVERRIDE_WARNING

#endif // NXNA_GRAPHICS_DIRECT3D11_D3D11INDEXBUFFER_H