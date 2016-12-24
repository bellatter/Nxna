#include "Utils.h"
#include <string>
#include <cstring>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#include <stdlib.h>
#endif

bool Utils::GetFullPath(const char* file, char* result, int maxLength)
{
#ifdef _WIN32
	GetFullPathName(file, maxLength, result, nullptr);
#else
	char* path = realpath(file, nullptr);
	if (path == nullptr)
		return false;

	strncpy(result, path, maxLength);
	free(path);
#endif

	return true;
}

bool Utils::ResolvePathRelativeTo(const char* relativePath, const char* originalPath, char* result, int maxLength)
{
	std::string fullPath = originalPath;
	if (fullPath[fullPath.length() - 1] != '/' &&
		fullPath[fullPath.length() - 1] != '\\')
		fullPath += '/';
	fullPath += relativePath;

	return GetFullPath(fullPath.c_str(), result, maxLength);
}