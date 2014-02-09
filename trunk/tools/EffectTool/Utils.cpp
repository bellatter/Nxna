#include "Utils.h"
#include <string>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#include <stdlib.h>
#endif

void Utils::GetFullPath(const char* file, char* result, int maxLength)
{
#ifdef _WIN32
	GetFullPathName(file, maxLength, result, nullptr);
#else
	char* path = realpath(file, nullptr);
	strncpy(result, path, maxLength);
	free(path);
#endif
}

bool Utils::ResolvePathRelativeTo(const char* relativePath, const char* originalPath, char* result, int maxLength)
{
	std::string fullPath = originalPath;
	if (fullPath[fullPath.length() - 1] != '/' &&
		fullPath[fullPath.length() - 1] != '\\')
		fullPath += '/';
	fullPath += relativePath;

	GetFullPath(fullPath.c_str(), result, maxLength);

	return true;
}