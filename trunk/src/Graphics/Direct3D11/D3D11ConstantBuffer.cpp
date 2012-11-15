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
	D3D11ConstantBuffer::D3D11ConstantBuffer(Direct3D11Device* device, bool vertex, int sizeInBytes,
		int* parameterIndices, int* parameterOffsets, int numParameters)
	{
		m_device = device;
		m_vertex = vertex;

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
		m_parameterIndices = new int[numParameters];
		m_parameterOffsets = new int[numParameters];
		m_numParameters = numParameters;
		memcpy(m_parameterIndices, parameterIndices, sizeof(int) * numParameters);
		memcpy(m_parameterOffsets, parameterOffsets, sizeof(int) * numParameters);
	}

	D3D11ConstantBuffer::~D3D11ConstantBuffer()
	{
		delete[] m_workingBuffer;
		delete[] m_parameterIndices;
		delete[] m_parameterOffsets;
		m_buffer->Release();
	}

	void D3D11ConstantBuffer::InjectParameterValuesIntoBuffer(std::vector<EffectParameter*>& parameters)
	{
		for (int i = 0; i < parameters.size(); i++)
		{
			int index = m_parameterIndices[i];
			int offset = m_parameterOffsets[i];

			setParameter(offset, parameters[i]);
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
		else
			context->PSSetConstantBuffers(slot, 1, &m_buffer);
	}

	void D3D11ConstantBuffer::setParameter(int offset, EffectParameter* param)
	{
		if (param->GetType() == EffectParameterType_Single)
		{
			param->GetValueSingleArray((float*)&m_workingBuffer[offset], param->GetNumElements());
		}
	}

	
}
}
}