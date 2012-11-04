#include "HlslEffect.h"
#include "Direct3D11Device.h"
#include "d3d11.h"

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

	void HlslEffect::AddPermutation(const byte* vertexBytecode, int vertexBytecodeLength, const byte* pixelBytecode, int pixelBytecodeLength)
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
		m_permutations.push_back(permutation);
	}
}
}
}