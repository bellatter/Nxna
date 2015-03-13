#include "MemoryAllocator.h"
#include <cstdlib>

namespace Nxna
{
	void* NxnaTempMemoryPool::m_memory = nullptr;
	size_t NxnaTempMemoryPool::m_memorySize = 0;

	void* NxnaTempMemoryPool::GetMemory(size_t amount)
	{
		if (amount > m_memorySize)
		{
			if (m_memory != nullptr)
			{
				delete[] m_memory;
			}

			m_memorySize = amount * 2;
			m_memory = new char[m_memorySize];
		}

		return m_memory;
	}
}