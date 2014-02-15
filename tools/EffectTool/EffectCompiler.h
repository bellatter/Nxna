#ifndef EFFECTCOMPILER_H
#define EFFECTCOMPILER_H

#include <vector>
#include "EffectXml.h"
#include "FilePaths.h"

class EffectCompiler
{
public:
	static void Compile(EffectXml* effect, const AbsoluteFilePath& pathToNxfxFolder);

private:
	static void loadExternalCode(const AbsoluteFilePath& pathToFile, EffectXml* effect, TechniqueMapXml* technique, bool vertex, const char* entryPoint, std::vector<unsigned char>& result);
	static bool isValidHLSLBytecodeHeader(std::vector<unsigned char>& data);
	static void compileHLSL(Profile::eProfile profile, bool vertex, const AbsoluteFilePath& pathToFile, const unsigned char* code, int codeLength, const char* entryPoint, std::vector<unsigned char>& result);
};

#endif // EFFECTCOMPILER_H