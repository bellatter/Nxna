#include "../../NxnaConfig.h"

#if !defined NXNA_DISABLE_D3D11

#include "D3D11ConstantBuffer.h"
#include "Direct3D11Device.h"
#include "HlslEffect.h"
#include "d3d11.h"

namespace Nxna
{
namespace Graphics
{
namespace Direct3D11
{
	D3D11ConstantBuffer::D3D11ConstantBuffer(Direct3D11Device* device, int index, bool vertex, bool pixel, int sizeInBytes, int numParameters)
	{
		m_device = device;
		m_index = index;
		m_vertex = vertex;
		m_pixel = pixel;

		D3D11_BUFFER_DESC desc;
		desc.ByteWidth = sizeInBytes;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		if (FAILED(static_cast<ID3D11Device*>(device->GetDevice())->CreateBuffer(&desc, nullptr, &m_buffer)))
			throw GraphicsException("Unable to create constant buffer", __FILE__, __LINE__);

		m_sizeInBytes = sizeInBytes;
		m_workingBuffer = new byte[sizeInBytes];
		m_numParameters = numParameters;
	}

	D3D11ConstantBuffer::~D3D11ConstantBuffer()
	{
		delete[] m_workingBuffer;
		m_buffer->Release();
	}

	void D3D11ConstantBuffer::InjectParameterValuesIntoBuffer(std::vector<EffectParameter*>& parameters)
	{
		for (unsigned int i = 0; i < parameters.size(); i++)
		{
			if (parameters[i]->GetConstantBufferIndex() == m_index)
			{
				if (parameters[i]->GetType() == EffectParameterType::Single)
				{
					parameters[i]->GetValueSingleArray((float*)&m_workingBuffer[parameters[i]->GetConstantBufferOffset()], parameters[i]->GetNumElements());
				}
			}
		}
	}
	
	void D3D11ConstantBuffer::Apply(int slot)
	{
		ID3D11DeviceContext* context = static_cast<ID3D11DeviceContext*>(m_device->GetDeviceContext());
		//context->UpdateSubresource(m_buffer, 0, nullptr, m_workingBuffer, 0, 0);

		D3D11_MAPPED_SUBRESOURCE r;
		if (FAILED(context->Map(m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &r)))
			throw GraphicsException("Unable to lock constant buffer", __FILE__, __LINE__);

		memcpy(r.pData, m_workingBuffer, m_sizeInBytes);

		context->Unmap(m_buffer, 0);

		if (m_vertex)
			context->VSSetConstantBuffers(slot, 1, &m_buffer);

		if (m_pixel)
			context->PSSetConstantBuffers(slot, 1, &m_buffer);
	}

	void D3D11ConstantBuffer::setParameter(int offset, EffectParameter* param)
	{
		if (param->GetType() == EffectParameterType::Single)
		{
			assert(offset >= 0);
			param->GetValueSingleArray((float*)&m_workingBuffer[offset], param->GetNumElements());
		}
	}

	
}
}
}

#endif
