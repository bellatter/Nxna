#ifndef NXNA_UTILS_H
#define NXNA_UTILS_H

#include "NxnaConfig.h"

namespace Nxna
{
	class Utils
	{
	public:
		static unsigned int CalcHash(const byte* buffer, int length)
		{
			// stolen from http://stackoverflow.com/questions/16340/how-do-i-generate-a-hashcode-from-a-byte-array-in-c-sharp/468084#468084

			const int p = 16777619;
			int hash = 2166136261;

			for (int i = 0; i < length; i++)
				hash = (hash ^ buffer[i]) * p;

			hash += hash << 13;
			hash ^= hash >> 7;
			hash += hash << 3;
			hash ^= hash >> 17;
			hash += hash << 5;
			return hash;
		}
	};

}

#endif // NXNA_UTILS_H
