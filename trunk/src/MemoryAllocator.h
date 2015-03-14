#ifndef NXNA_MEMORYALLOCATOR_H
#define NXNA_MEMORYALLOCATOR_H

#include <cstdlib>

namespace Nxna
{
	class NxnaTempMemoryPool
	{
		static char* m_memory;
		static size_t m_memorySize;

	public:
		static void* GetMemory(size_t amount);
	};
}

#endif // NXNA_MEMORYALLOCATOR_H
