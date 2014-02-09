#include <fstream>
#include "EffectCompiler.h"
#include "EffectXml.h"
#include "EffectToolException.h"


void EffectCompiler::Compile(EffectXml* effect)
{
	auto& techniques = effect->GetTechniqueMaps();
	auto& shaders = effect->GetShaders();

	// iterate through the techniques and find the associated shader
	for (auto itr = techniques.begin(); itr != techniques.end(); itr++)
	{
		int vertexShader = effect->FindShaderByName((*itr).VertexShader.c_str());
		int pixelShader = effect->FindShaderByName((*itr).PixelShader.c_str());

		if (vertexShader >= 0 && shaders[vertexShader].SourceFile.empty() == false)
		{
			loadExternalCode((*itr).ProfileType, shaders[vertexShader].SourceFile, shaders[vertexShader].SourceCode);
		}

		if (pixelShader >= 0 && shaders[pixelShader].SourceFile.empty() == false)
		{
			loadExternalCode((*itr).ProfileType, shaders[vertexShader].SourceFile, shaders[vertexShader].SourceCode);
		}
	}
}

void EffectCompiler::loadExternalCode(Profile::eProfile profile, const std::string& filename, std::vector<unsigned char>& result)
{
	// load the source file
	std::ifstream input;
	input.open(filename.c_str(), std::ios::binary);

	if (input.is_open() == false)
		throw EffectToolException("Unable to load external shader", filename.c_str());

	// get the file size
	input.seekg(0, std::ios::end);
	auto size = input.tellg();
	input.seekg(0, std::ios::beg);

	// read the file
	result.resize(size);
	input.read((char*)result.data(), size);
	input.close();

	if ((profile & Profile::ANY_HLSL) == Profile::ANY_HLSL)
	{
		// this may be uncompiled HLSL. Check to see.
		if (isValidHLSLBytecodeHeader(result) == false)
		{
			// This doesn't seem to be HLSL bytecode. let's try compiling using fxc.
			compileHLSL(profile, filename, result);
		}
	}
}

bool EffectCompiler::isValidHLSLBytecodeHeader(std::vector<unsigned char>& data)
{
	if (data.size() >= 4)
	{
		// is this SM4 or 5 bytecode?
		if (data[0] == 'D' && data[1] == 'X' && data[2] == 'B' && data[3] == 'C')
			return true;

		// perhaps this is DX9-level bytecode?
		unsigned int token;
		memcpy(&token, data.data(), 4);
		if ((token & 0xFFFF0000) == 0xFFFE0000 // vertex shader
			|| (token & 0xFFFF0000) == 0xFFFF0000) // pixel shader
			return true;
	}

	return false;
}

void EffectCompiler::compileHLSL(Profile::eProfile profile, const std::string& filename, std::vector<unsigned char>& result)
{
	throw EffectToolException("HLSL compiling is not implemented yet");
}