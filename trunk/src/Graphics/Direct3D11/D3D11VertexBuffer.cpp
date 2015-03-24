#include "../../NxnaConfig.h"

#if !defined NXNA_DISABLE_D3D11

#include <cassert>
#include "Direct3D11Device.h"
#include "D3D11VertexBuffer.h"
#include "../VertexDeclaration.h"
#include "d3d11.h"

namespace Nxna
{
namespace Graphics
{
namespace Direct3D11
{
	D3D11VertexBuffer::D3D11VertexBuffer(bool dynamic, Direct3D11Device* device, const VertexDeclaration* vertexDeclaration, int vertexCount, BufferUsage usage)
		: m_declaration(*vertexDeclaration)
	{
		m_dynamic = dynamic;
		m_device = device;
		m_vertexCount = vertexCount;

		m_buffer = nullptr;
	}

	void D3D11VertexBuffer::SetData(int offsetInBytes, void* data, int numBytes)
	{
		if (m_dynamic)
			setDataDynamic(offsetInBytes, data, numBytes);
		else
			setDataStatic(offsetInBytes, data, numBytes);
	}

	void D3D11VertexBuffer::setDataStatic(int offsetInBytes, void* data, int numBytes)
	{
		int capacity = m_declaration.GetStride() * m_vertexCount;

		if (numBytes > capacity)
			throw GraphicsException("Too many vertices", __FILE__, __LINE__);

		if (numBytes > 0 && m_buffer == nullptr)
		{
			D3D11_BUFFER_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.ByteWidth = capacity;
			desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			desc.CPUAccessFlags = 0;

			if (FAILED(static_cast<ID3D11Device*>(m_device->GetDevice())->CreateBuffer(&desc, nullptr, (ID3D11Buffer**)&m_buffer)))
				throw GraphicsException("Unable to create vertex buffer");
		}

		D3D11_MAPPED_SUBRESOURCE mappedBuffer;

		ID3D11DeviceContext* deviceContext = static_cast<ID3D11DeviceContext*>(m_device->GetDeviceContext());

		D3D11_BOX box;
		box.top = 0;
		box.front = 0;
		box.back = 1;
		box.bottom = 1;
		box.left = offsetInBytes;
		box.right = offsetInBytes + numBytes;
		deviceContext->UpdateSubresource(static_cast<ID3D11Buffer*>(m_buffer), 0, &box, data, 1, 0);

	}

	void D3D11VertexBuffer::setDataDynamic(int offsetInBytes, void* data, int numBytes)
	{
		// TODO
		assert(offsetInBytes == 0);

		int capacity = m_declaration.GetStride() * m_vertexCount;

		if (numBytes > capacity)
			throw GraphicsException("Too many vertices", __FILE__, __LINE__);

		if (numBytes > 0 && m_buffer == nullptr)
		{
			D3D11_BUFFER_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.Usage = D3D11_USAGE_DYNAMIC;
			desc.ByteWidth = capacity;
			desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

			if (FAILED(static_cast<ID3D11Device*>(m_device->GetDevice())->CreateBuffer(&desc, nullptr, (ID3D11Buffer**)&m_buffer)))
				throw GraphicsException("Unable to create dynamic vertex buffer");
		}

		D3D11_MAPPED_SUBRESOURCE mappedBuffer;

		ID3D11DeviceContext* deviceContext = static_cast<ID3D11DeviceContext*>(m_device->GetDeviceContext());

		if (FAILED(deviceContext->Map(static_cast<ID3D11Buffer*>(m_buffer), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer)))
			throw GraphicsException("Unable to create dynamic vertex buffer");

		memcpy(mappedBuffer.pData, data, numBytes);

		deviceContext->Unmap(static_cast<ID3D11Buffer*>(m_buffer), 0);
	}
}
}
}

#endif