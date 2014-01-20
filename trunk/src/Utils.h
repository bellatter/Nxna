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

		static unsigned int GetUTF8Character(const char* string, int position, int* result)
		{
			unsigned int firstByte = (unsigned char)string[position];
			int totalBytesToRead;

			
			if (firstByte == 0)
				return 0;
			else if ((firstByte & 0x80) == 0x00)
			{
				// early out. Single character.
				*result = firstByte;
				return 1;
			}
			else if ((firstByte & 0xE0) == 0xC0)
			{
				*result = (firstByte & 0x1F) << 6;
				totalBytesToRead = 2;
			}
			else if ((firstByte & 0xF0) == 0xE0)
			{
				*result = (firstByte & 0x0F) << 12;
				totalBytesToRead = 3;
			}
			else if ((firstByte & 0xF8) == 0xF0)
			{
				*result = (firstByte & 0x07) << 18;
				totalBytesToRead = 4;
			}
			else if ((firstByte & 0xFC) == 0xF8)
			{
				*result = (firstByte & 0x03) << 24;
				totalBytesToRead = 5;
			}
			else if ((firstByte & 0xFE) == 0xFC)
			{
				*result = (firstByte & 0x01) << 30;
				totalBytesToRead = 6;
			}
			else
				// woah! this shouldn't happen!!
				return 0;

			for (int i = 1; i < totalBytesToRead; i++)
			{
				unsigned int nextByte = (unsigned char)string[position + i];
				if (nextByte == 0)
				{
					// well that was unexpected.
					return 0;
				}

				int toShift = (6 * (totalBytesToRead - i - 1));
				*result |= ((nextByte & 0x3F) << toShift);
			}

			return totalBytesToRead;
		}
	};

}

#endif // NXNA_UTILS_H
