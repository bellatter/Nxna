#include "VertexDeclaration.h"
#include "../Utils.h"

namespace Nxna
{
namespace Graphics
{
	unsigned int VertexDeclaration::calcHash()
	{
		static byte hashBuffer[16 * 4];

		for (int i = 0; i < m_numElements; i++)
		{
			hashBuffer[i * 4 + 0] = (m_elements[i].Offset & 0xff);
			hashBuffer[i * 4 + 1] = (m_elements[i].Offset & 0xff00) >> 8;
			hashBuffer[i * 4 + 2] = ((int)m_elements[i].ElementFormat & 0x7f) | (((int)m_elements[i].ElementUsage & 0x7f) << 4);
			hashBuffer[i * 4 + 3] = (m_elements[i].UsageIndex & 0xff);
		}

		return Utils::CalcHash(hashBuffer, m_numElements * 4);
	}
}
}
