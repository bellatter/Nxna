#ifdef _WIN32

#include <fstream>
#include <cassert>
#include "HLSLIncluder.h"

HLSLIncluder::HLSLIncluder(const AbsoluteFilePath& pathToParentFile)
{
	m_baseDir = AbsoluteFilePath::GetDirectoryName(pathToParentFile);
}

HRESULT HLSLIncluder::Open(D3D_INCLUDE_TYPE includeType, LPCSTR filename, LPCVOID parentData, LPCVOID* data, unsigned int* bytes)
{
	AbsoluteFilePath fileToOpen = AbsoluteFilePath::ConvertRelativePath(m_baseDir, filename);

	// Open file
	std::ifstream file(fileToOpen.GetPath().c_str(), std::ios::binary);
	if(!file.is_open())
	{
		assert("boom" && 0);
		*data = nullptr;
		*bytes = 0;
		return S_OK;
	}

	// Get filesize
	file.seekg(0, std::ios::end);
	UINT size = static_cast<UINT>(file.tellg());
	file.seekg(0, std::ios::beg);

	// Create buffer and read file
	char* buffer = new char[size + 1];
	file.read(buffer, size);
	file.close();
	buffer[size] = 0;

	*data = buffer;
	*bytes = size;
	m_buffers.push_back(buffer);

	return S_OK;
}

HRESULT HLSLIncluder::Close(LPCVOID data)
{
	for (auto itr = m_buffers.begin(); itr != m_buffers.end(); itr++)
		if ((*itr) == data)
		{
			auto buffer = *itr;
			delete[] buffer;

			m_buffers.erase(itr);
			break;
		}

	return S_OK;
}

#endif