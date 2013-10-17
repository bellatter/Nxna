#include "../../NxnaConfig.h"

#if !defined NXNA_DISABLE_D3D11

#include "D3D11IndexBuffer.h"
#include "../GraphicsDevice.h"
#include "d3d11.h"

namespace Nxna
{
namespace Graphics
{
namespace Direct3D11
{
	D3D11IndexBuffer::D3D11IndexBuffer(void* d3d11Device, IndexElementSize elementSize)
		: Pvt::IIndexBufferPimpl(elementSize)
	{
		m_d3d11Device = d3d11Device;
		m_indexBuffer = nullptr;
		m_elementSize = elementSize;
	}

	D3D11IndexBuffer::~D3D11IndexBuffer()
	{
		static_cast<ID3D11Buffer*>(m_indexBuffer)->Release();
	}

	void D3D11IndexBuffer::SetData(void* data, int indexCount)
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.ByteWidth = (int)m_elementSize * indexCount;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA initData;
		ZeroMemory(&initData, sizeof(initData));
		initData.pSysMem = data;

		if (FAILED(static_cast<ID3D11Device*>(m_d3d11Device)->CreateBuffer(&desc, &initData, (ID3D11Buffer**)&m_indexBuffer)))
			throw GraphicsException("Unable to create Direct3D index buffer");
	}
}
}
}

#endif