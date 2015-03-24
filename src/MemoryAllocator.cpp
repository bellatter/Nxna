#include "MemoryAllocator.h"

#ifdef NXNA_DEBUG_TEMPMEMORYPOOL
#include <cassert>
#endif

namespace Nxna
{
	char* NxnaTempMemoryPool::m_memory = nullptr;
	size_t NxnaTempMemoryPool::m_memorySize = 0;

#ifdef NXNA_DEBUG_TEMPMEMORYPOOL
	bool NxnaTempMemoryPool::m_aquired = false;
#endif

	void* NxnaTempMemoryPool::GetMemory(size_t amount)
	{
#ifdef NXNA_DEBUG_TEMPMEMORYPOOL
		assert(m_aquired == false && "Someone didn't release the memory pool!");

		m_aquired = true;

		m_memorySize = amount;
		m_memory = new char[amount];

		return m_memory;
#else
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
#endif
	}

#ifdef NXNA_DEBUG_TEMPMEMORYPOOL
	void NxnaTempMemoryPool::ReleaseMemory()
	{
		assert(m_aquired == true && "Someone is trying to release pool memory, but it isn't aquired!");

		m_aquired = false;

		delete[] m_memory;
	}
#endif
}