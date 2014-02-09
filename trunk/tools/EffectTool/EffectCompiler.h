#ifndef EFFECTCOMPILER_H
#define EFFECTCOMPILER_H

#include <vector>
#include "EffectXml.h"

class EffectCompiler
{
public:
	void Compile(EffectXml* effect);

private:
	void loadExternalCode(Profile::eProfile profile, const std::string& filename, std::vector<unsigned char>& result);
	bool isValidHLSLBytecodeHeader(std::vector<unsigned char>& data);
	void compileHLSL(Profile::eProfile profile, const std::string& filename, std::vector<unsigned char>& result);
};

#endif // EFFECTCOMPILER_H