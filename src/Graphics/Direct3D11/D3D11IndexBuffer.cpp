#include "../../NxnaConfig.h"

#if !defined NXNA_DISABLE_D3D11

#include "Direct3D11Device.h"
#include "D3D11IndexBuffer.h"
#include "../GraphicsDevice.h"
#include "d3d11.h"

namespace Nxna
{
namespace Graphics
{
namespace Direct3D11
{
	D3D11IndexBuffer::D3D11IndexBuffer(Direct3D11Device* d3d11Device, IndexElementSize elementSize)
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

	void D3D11IndexBuffer::SetData(int offsetInBytes, void* data, int indexCount)
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.ByteWidth = (int)m_elementSize * indexCount;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.CPUAccessFlags = 0;

		if (FAILED(static_cast<ID3D11Device*>(m_d3d11Device->GetDevice())->CreateBuffer(&desc, nullptr, (ID3D11Buffer**)&m_indexBuffer)))
			throw GraphicsException("Unable to create Direct3D index buffer");

		ID3D11DeviceContext* deviceContext = static_cast<ID3D11DeviceContext*>(m_d3d11Device->GetDeviceContext());

		D3D11_BOX box;
		box.top = 0;
		box.front = 0;
		box.back = 1;
		box.bottom = 1;
		box.left = offsetInBytes;
		box.right = offsetInBytes + indexCount * (int)m_elementSize;

		deviceContext->UpdateSubresource(static_cast<ID3D11Buffer*>(m_indexBuffer), 0, &box, data, 1, 0);
	}
}
}
}

#endif