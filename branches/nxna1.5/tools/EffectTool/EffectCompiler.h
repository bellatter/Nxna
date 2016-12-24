#ifndef EFFECTCOMPILER_H
#define EFFECTCOMPILER_H

#include <vector>
#include "EffectXml.h"
#include "FilePaths.h"

class EffectCompiler
{
public:
	static void Compile(EffectXml* effect, const AbsoluteFilePath& pathToNxfxFile);

private:

	struct CompilerContext
	{
		EffectXml* Effect;
		const AbsoluteFilePath& PathToNxfxFile;
	};

	static void loadExternalCode(const AbsoluteFilePath& pathToFile, std::vector<unsigned char>& result);
	static bool isValidHLSLBytecodeHeader(const std::vector<unsigned char>& data);
	static void compileIfNeeded(const std::vector<unsigned char>& source, bool vertex, EffectXml* effect, TechniqueMapXml& technique, const AbsoluteFilePath& pathToNxfxFolder, const AbsoluteFilePath* pathToFile);
	static void compileHLSL(Profile::eProfile profile, bool vertex, const AbsoluteFilePath& pathToNxfxFolder, const AbsoluteFilePath* pathToFile, const unsigned char* code, int codeLength, const char* entryPoint, std::vector<unsigned char>& result);
};

#endif // EFFECTCOMPILER_H