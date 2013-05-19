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
	D3D11VertexBuffer::D3D11VertexBuffer(Direct3D11Device* device, const VertexDeclaration* vertexDeclaration, int vertexCount, BufferUsage usage)
		: VertexBuffer(device)
	{
		m_declaration = vertexDeclaration;
		m_vertexCount = vertexCount;

		m_buffer = nullptr;
	}

	void D3D11VertexBuffer::SetData(void* data, int vertexCount)
	{
		//assert(vertexCount == m_vertexCount);

		int numBytes = m_declaration->GetStride() * m_vertexCount;

		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = m_declaration->GetStride() * m_vertexCount;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = 0;
		D3D11_SUBRESOURCE_DATA initData;
		ZeroMemory(&initData, sizeof(initData));
		initData.pSysMem = data;
		if (FAILED(static_cast<ID3D11Device*>(static_cast<Direct3D11Device*>(GetGraphicsDevice())->GetDevice())->CreateBuffer(&desc, &initData, (ID3D11Buffer**)&m_buffer)))
			throw GraphicsException("Unable to create vertex buffer");
	}
	/*
	void GlVertexBuffer::Bind() const
	{
		GlException::ThrowIfError(__FILE__, __LINE__);
		glBindBuffer(GL_ARRAY_BUFFER, m_buffer);

		GlException::ThrowIfError(__FILE__, __LINE__);
	}*/

	D3D11DynamicVertexBuffer::D3D11DynamicVertexBuffer(Direct3D11Device* device, const VertexDeclaration* vertexDeclaration, int vertexCount, BufferUsage usage)
		: D3D11VertexBuffer(device, vertexDeclaration, vertexCount, usage)
		, DynamicVertexBuffer(device)
	{
		
	}

	void D3D11DynamicVertexBuffer::SetData(void* data, int vertexCount)
	{
		if (vertexCount > m_vertexCount)
			throw GraphicsException("Too many vertices", __FILE__, __LINE__);

		if (vertexCount > 0 && m_buffer == nullptr)
		{
			int numBytes = m_declaration->GetStride() * m_vertexCount;

			D3D11_BUFFER_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.Usage = D3D11_USAGE_DYNAMIC;
			desc.ByteWidth = numBytes;
			desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

			if (FAILED(static_cast<ID3D11Device*>(static_cast<Direct3D11Device*>(D3D11VertexBuffer::GetGraphicsDevice())->GetDevice())->CreateBuffer(&desc, nullptr, (ID3D11Buffer**)&m_buffer)))
				throw GraphicsException("Unable to create dynamic vertex buffer");
		}

		int numBytes = m_declaration->GetStride() * vertexCount;

		D3D11_MAPPED_SUBRESOURCE mappedBuffer;

		ID3D11DeviceContext* deviceContext = static_cast<ID3D11DeviceContext*>(static_cast<Direct3D11Device*>(D3D11VertexBuffer::GetGraphicsDevice())->GetDeviceContext());

		if (FAILED(deviceContext->Map(static_cast<ID3D11Buffer*>(m_buffer), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer)))
			throw GraphicsException("Unable to create dynamic vertex buffer");

		memcpy(mappedBuffer.pData, data, numBytes);

		deviceContext->Unmap(static_cast<ID3D11Buffer*>(m_buffer), 0);
	}
}
}
}

#endif