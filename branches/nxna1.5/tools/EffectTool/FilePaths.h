#ifndef FILEPATHS_H
#define FILEPATHS_H

#include <string>

class RelativeFilePath
{
	std::string m_path;

public:
	RelativeFilePath(const char* path);

	const std::string& GetPath() const { return m_path; }
};

class AbsoluteFilePath
{
	std::string m_path;

public:
	AbsoluteFilePath();
	AbsoluteFilePath(const char* path);
	
	const std::string& GetPath() const { return m_path; }

	static bool IsAbsolutePath(const char* path);
	static AbsoluteFilePath ConvertRelativePath(const AbsoluteFilePath& root, const RelativeFilePath& relativePath);
	static AbsoluteFilePath ConvertRelativePath(const AbsoluteFilePath& root, const char* relativePath);
	static AbsoluteFilePath GetDirectoryName(const AbsoluteFilePath& path);
};



#endif // FILEPATHS_H
