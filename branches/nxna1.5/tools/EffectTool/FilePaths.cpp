#include "FilePaths.h"
#include "Utils.h"
#include <cassert>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Shlwapi.h>
#endif

RelativeFilePath::RelativeFilePath(const char* path)
{
	assert(AbsoluteFilePath::IsAbsolutePath(path) == false);

	m_path = path;
}

AbsoluteFilePath::AbsoluteFilePath()
{
}

AbsoluteFilePath::AbsoluteFilePath(const char* path)
{
	assert(AbsoluteFilePath::IsAbsolutePath(path) == true);

	m_path = path;
}

bool AbsoluteFilePath::IsAbsolutePath(const char* path)
{
#ifdef _WIN32
	return PathIsRelativeA(path) == 0;
#else
	if (path[0] == '/')
		return true;

	return false;
#endif
}

AbsoluteFilePath AbsoluteFilePath::ConvertRelativePath(const AbsoluteFilePath& root, const RelativeFilePath& relativePath)
{
	char buffer[512];
	Utils::ResolvePathRelativeTo(relativePath.GetPath().c_str(), root.GetPath().c_str(), buffer, 512);
	buffer[511] = 0;

	return AbsoluteFilePath(buffer);
}

AbsoluteFilePath AbsoluteFilePath::ConvertRelativePath(const AbsoluteFilePath& root, const char* relativePath)
{
	char buffer[512];
	Utils::ResolvePathRelativeTo(relativePath, root.GetPath().c_str(), buffer, 512);
	buffer[511] = 0;

	return AbsoluteFilePath(buffer);
}

AbsoluteFilePath AbsoluteFilePath::GetDirectoryName(const AbsoluteFilePath& path)
{
	std::string pathToBlah;
	auto lastSlash = path.GetPath().find_last_of("/\\");
	if (lastSlash == std::string::npos)
		pathToBlah = ".";
	else
		pathToBlah = path.GetPath().substr(0, lastSlash);

	return AbsoluteFilePath(pathToBlah.c_str());
}