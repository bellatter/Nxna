#ifndef NXNA_GRAPHICS_DIRECT3D11_D3D11CONSTANTBUFFER_H
#define NXNA_GRAPHICS_DIRECT3D11_D3D11CONSTANTBUFFER_H

#include "../../NxnaConfig.h"
#include "../IConstantBufferPmpl.h"
#include <vector>

struct ID3D11Buffer;

namespace Nxna
{
namespace Graphics
{
	class EffectParameter;

namespace Direct3D11
{
	class Direct3D11Device;

	class D3D11ConstantBuffer : public IConstantBufferPmpl
	{
		Direct3D11Device* m_device;
		ID3D11Buffer* m_buffer;
		bool m_vertex;
		bool m_pixel;
		byte* m_workingBuffer;
		int* m_parameterIndices;
		int* m_parameterOffsets;
		int m_numParameters;
		int m_sizeInBytes;


	public:
		D3D11ConstantBuffer(Direct3D11Device* device, bool vertex, bool pixel, int sizeInBytes,
			int* parameterIndices, int* parameterOffsets, int numParameters);
		virtual ~D3D11ConstantBuffer();

		void InjectParameterValuesIntoBuffer(std::vector<EffectParameter*>& parameters);
		void Apply(int slot);


	private:
		void setParameter(int offset, EffectParameter* param);
	};
}
}
}

#endif // NXNA_GRAPHICS_DIRECT3D11_D3D11CONSTANTBUFFER_H