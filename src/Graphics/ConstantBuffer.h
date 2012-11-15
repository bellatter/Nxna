#ifndef NXNA_GRAPHICS_CONSTANTBUFFER_H
#define NXNA_GRAPHICS_CONSTANTBUFFER_H

namespace Nxna
{
namespace Graphics
{
	class IConstantBufferPmpl;

	class ConstantBuffer
	{
		IConstantBufferPmpl* m_pmpl;

	public:
		ConstantBuffer(IConstantBufferPmpl* pmpl) { m_pmpl = pmpl; }

		IConstantBufferPmpl* GetPmpl() { return m_pmpl; }
	};
}
}

#endif // NXNA_GRAPHICS_CONSTANTBUFFER_H
