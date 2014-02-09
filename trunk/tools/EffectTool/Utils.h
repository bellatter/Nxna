#ifndef UTILS_H
#define UTILS_H

class Utils
{
public:
	static void GetFullPath(const char* file, char* result, int maxLength);

	static bool ResolvePathRelativeTo(const char* relativePath, const char* originalPath, char* result, int maxLength);
};

#endif // UTILS_H
