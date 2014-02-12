#ifndef EFFECTCOMPILER_H
#define EFFECTCOMPILER_H

#include <vector>
#include "EffectXml.h"

class EffectCompiler
{
public:
	static void Compile(EffectXml* effect, const char* pathToNxfxFolder);

private:
	static void loadExternalCode(const char* pathToNxfxFolder, EffectXml* effect, TechniqueMapXml* technique, bool vertex, const std::string& filename, const char* entryPoint, std::vector<unsigned char>& result);
	static bool isValidHLSLBytecodeHeader(std::vector<unsigned char>& data);
	static void compileHLSL(Profile::eProfile profile, bool vertex, const unsigned char* code, int codeLength, const char* entryPoint, std::vector<unsigned char>& result);
};

#endif // EFFECTCOMPILER_H