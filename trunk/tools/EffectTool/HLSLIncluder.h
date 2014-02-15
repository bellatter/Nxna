#ifndef HLSLINCLUDER_H
#define HLSLINCLUDER_H

#include <D3Dcommon.h>
#include <string>
#include <vector>
#include "FilePaths.h"

class HLSLIncluder : public ID3DInclude
{
	AbsoluteFilePath m_baseDir;
	std::vector<char*> m_buffers;

public:
	HLSLIncluder(const AbsoluteFilePath& pathToNxfx);

	HRESULT __stdcall Open(D3D_INCLUDE_TYPE includeType, LPCSTR filename, LPCVOID parentData, LPCVOID* data, unsigned int* bytes);

	HRESULT __stdcall Close(LPCVOID data);
};

#endif // HLSLINCLUDER_H
