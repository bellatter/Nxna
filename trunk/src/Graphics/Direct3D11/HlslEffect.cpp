#include "../../NxnaConfig.h"

#if !defined NXNA_DISABLE_D3D11

#include "HlslEffect.h"
#include "Direct3D11Device.h"
#include "d3d11.h"
#include "D3D11Shader.h"
#include "D3D11ConstantBuffer.h"
#include "D3D11Texture2D.h"
#include "../../Utils.h"

namespace Nxna
{
namespace Graphics
{
namespace Direct3D11
{
	HlslEffect::HlslEffect(Direct3D11Device* device, Effect* parent)
		: Pvt::IEffectPimpl(parent)
	{
		m_device = device;
	}

	HlslEffect::~HlslEffect()
	{
		// TODO: release all the shader permutations
	}

	EffectTechnique* HlslEffect::CreateProgram(const char* name, bool hidden, const byte* vertexBytecode, int vertexBytecodeLength,
		const byte* pixelBytecode, int pixelBytecodeLength)
	{
		if (vertexBytecode != nullptr || pixelBytecode != nullptr)
		{
			ID3D11Device* d3d11Device = static_cast<ID3D11Device*>(static_cast<Direct3D11Device*>(m_device)->GetDevice());

			HlslPermutation permutation;
			auto r = d3d11Device->CreateVertexShader(vertexBytecode, vertexBytecodeLength, nullptr, (ID3D11VertexShader**)&permutation.D3DVertexShader);
			if (FAILED(r))
				throw GraphicsException("Unable to add permutation to HLSL effect", __FILE__, __LINE__);

			if (FAILED(d3d11Device->CreatePixelShader(pixelBytecode, pixelBytecodeLength, nullptr, (ID3D11PixelShader**)&permutation.D3DPixelShader)))
			{
				static_cast<ID3D11VertexShader*>(permutation.D3DVertexShader)->Release();
				throw GraphicsException("Unable to add permutation to HLSL effect", __FILE__, __LINE__);
			}

			permutation.Hash = Utils::CalcHash(vertexBytecode, vertexBytecodeLength);
			permutation.VertexBytecode = new byte[vertexBytecodeLength];
			memcpy(permutation.VertexBytecode, vertexBytecode, vertexBytecodeLength);
			permutation.VertexBytecodeLength = vertexBytecodeLength;

			m_permutations.push_back(permutation);
		}

		return CreateTechnique(name, hidden);
	}

	void HlslEffect::AddAttributeToProgram(int programIndex, const char* name, EffectParameterType type, int numElements, Semantic semantic, int usageIndex)
	{
		// nothing to do. All the attributes are already automagically set up for us. 
	}

	void HlslEffect::AddConstantBuffer(bool vertex, bool pixel, int sizeInBytes, int numParameters)
	{
		ConstantBuffer cbuffer(new D3D11ConstantBuffer(m_device, vertex, pixel, sizeInBytes, numParameters));
		GetConstantBuffers().push_back(cbuffer);
	}

	EffectParameter* HlslEffect::AddParameter(const char* name, EffectParameterType type, int numElements, int constantBufferIndex, int constantBufferConstantIndex, int constantBufferOffset)
	{
		EffectParameter* parameter = CreateParameter(m_parent, type, numElements, nullptr, name);

		m_parameters.insert(ParamMap::value_type(parameter->Name.c_str(), parameter));

		m_parameterList.push_back(parameter);

		if (type != EffectParameterType::Texture
			&& type != EffectParameterType::Texture1D
			&& type != EffectParameterType::Texture2D
			&& type != EffectParameterType::Texture3D
			&& type != EffectParameterType::TextureCube)
			static_cast<D3D11ConstantBuffer*>(GetConstantBuffers()[constantBufferIndex].GetPmpl())->SetParameterOffset(constantBufferConstantIndex, constantBufferOffset);

		return parameter;
	}

	int HlslEffect::ScoreProfile(ShaderProfile profile)
	{
		if (profile == ShaderProfile::HLSL_4_0_level_9_1)
			return 1;

		return 0;
	}

	unsigned int HlslEffect::GetHash(unsigned int program)
	{
		if (program < m_permutations.size())
			return m_permutations[program].Hash;

		return 0;
	}

	void HlslEffect::GetBytecode(unsigned int program, byte** bytecode, int* length)
	{
		if (program < m_permutations.size())
		{
			*bytecode = m_permutations[program].VertexBytecode;
			*length = m_permutations[program].VertexBytecodeLength;
		}
	}

	void HlslEffect::SetConstantBuffers()
	{
		for (unsigned int i = 0; i < m_cbuffers.size(); i++)
		{
			static_cast<D3D11ConstantBuffer*>(m_cbuffers[i].GetPmpl())->InjectParameterValuesIntoBuffer(m_parameterList);
			static_cast<D3D11ConstantBuffer*>(m_cbuffers[i].GetPmpl())->Apply(i);
		}
	}

	void HlslEffect::ApplyProgram(int programIndex)
	{
		ID3D11VertexShader* vertexShader = static_cast<ID3D11VertexShader*>(m_permutations[programIndex].D3DVertexShader);
		ID3D11PixelShader* pixelShader = static_cast<ID3D11PixelShader*>(m_permutations[programIndex].D3DPixelShader);

		ID3D11DeviceContext* context = static_cast<ID3D11DeviceContext*>(static_cast<Direct3D11Device*>(m_device)->GetDeviceContext());
		context->VSSetShader(vertexShader, nullptr, 0);
		context->PSSetShader(pixelShader, nullptr, 0);

		// while we're at it, let's set the textures
		int textureCount = 0;
		for (unsigned int i = 0; i < m_parameterList.size(); i++)
		{
			if (m_parameterList[i]->GetType() == EffectParameterType::Texture2D)
			{
				Texture2D* value = m_parameterList[i]->GetValueTexture2D();
				if (value != nullptr)
				{
					D3D11Texture2D* texture = static_cast<D3D11Texture2D*>(value->GetPimpl());
					ID3D11ShaderResourceView* d3dTex = texture->GetShaderResourceView();
					context->PSSetShaderResources(textureCount++, 1, &d3dTex);
				}
			}
		}
	}

	void HlslEffect::Apply(int techniqueIndex)
	{
		m_device->SetCurrentEffect(this, techniqueIndex);
	}
}
}
}

#endif
