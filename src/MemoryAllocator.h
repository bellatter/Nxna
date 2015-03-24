#ifndef NXNA_MEMORYALLOCATOR_H
#define NXNA_MEMORYALLOCATOR_H

#include <cstdlib>

namespace Nxna
{
	class NxnaTempMemoryPool
	{
		static char* m_memory;
		static size_t m_memorySize;

#ifdef NXNA_DEBUG_TEMPMEMORYPOOL
		static bool m_aquired;
#endif

	public:
		static void* GetMemory(size_t amount);

#ifdef NXNA_DEBUG_TEMPMEMORYPOOL
		static void ReleaseMemory();
#else
		static void ReleaseMemory() {}
#endif
	};
}

#endif // NXNA_MEMORYALLOCATOR_H
