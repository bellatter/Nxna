#include "../../NxnaConfig.h"

#if defined NXNA_PLATFORM_WIN32

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
	HlslEffect::HlslEffect(Direct3D11Device* device)
	{
		m_device = device;
	}

	HlslEffect::~HlslEffect()
	{
		// TODO: release all the shader permutations
	}

	void HlslEffect::AddPermutation(const byte* vertexBytecode, int vertexBytecodeLength,
		const byte* pixelBytecode, int pixelBytecodeLength)
	{
		ID3D11Device* d3d11Device = static_cast<ID3D11Device*>(static_cast<Direct3D11Device*>(m_device)->GetDevice());

		HlslPermutation permutation;
		if (FAILED(d3d11Device->CreateVertexShader(vertexBytecode, vertexBytecodeLength, nullptr, (ID3D11VertexShader**)&permutation.D3DVertexShader)))
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

	void HlslEffect::Apply()
	{
		// TODO: determine which program to use
		static_cast<Direct3D11Device*>(m_device)->SetCurrentEffect(this, 0);
	}

	unsigned int HlslEffect::GetHash(int program)
	{
		if (program >= 0 && program < m_permutations.size())
			return m_permutations[program].Hash;

		return 0;
	}

	void HlslEffect::GetBytecode(int program, byte** bytecode, int* length)
	{
		if (program >= 0 && program < m_permutations.size())
		{
			*bytecode = m_permutations[program].VertexBytecode;
			*length = m_permutations[program].VertexBytecodeLength;
		}
	}

	void HlslEffect::SetConstantBuffers()
	{
		for (int i = 0; i < m_cbuffers.size(); i++)
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
		for (int i = 0; i < m_parameterList.size(); i++)
		{
			if (m_parameterList[i]->GetType() == EffectParameterType_Texture2D)
			{
				D3D11Texture2D* texture = static_cast<D3D11Texture2D*>(m_parameterList[i]->GetValueTexture2D());
				ID3D11ShaderResourceView* d3dTex = (ID3D11ShaderResourceView*)texture->GetHandle();
				context->PSSetShaderResources(0, 1, &d3dTex);
			}
		}
	}
}
}
}

#endif
