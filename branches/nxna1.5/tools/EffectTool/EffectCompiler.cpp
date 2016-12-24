#include <fstream>
#include <cstring>
#include "EffectCompiler.h"
#include "EffectXml.h"
#include "EffectToolException.h"
#include "Utils.h"

#ifdef _WIN32
#include <D3Dcompiler.h>
#include "HLSLIncluder.h"
#endif


void EffectCompiler::Compile(EffectXml* effect, const AbsoluteFilePath& pathToNxfxFolder)
{
	auto& techniques = effect->GetTechniqueMaps();
	auto& shaders = effect->GetShaders();

	// iterate through the techniques and find the associated shader
	for (auto itr = techniques.begin(); itr != techniques.end(); itr++)
	{
		int vertexShader = effect->FindShaderByName((*itr).VertexShader.c_str());
		int pixelShader = effect->FindShaderByName((*itr).PixelShader.c_str());

		if (vertexShader >= 0)
		{
			if (shaders[vertexShader].SourceFile.empty() == false)
			{
				AbsoluteFilePath pathToFile;
				if (AbsoluteFilePath::IsAbsolutePath(shaders[vertexShader].SourceFile.c_str()))
					pathToFile = AbsoluteFilePath(shaders[vertexShader].SourceFile.c_str());
				else
					pathToFile = AbsoluteFilePath::ConvertRelativePath(pathToNxfxFolder, shaders[vertexShader].SourceFile.c_str());

				loadExternalCode(pathToFile, shaders[vertexShader].SourceCode);
				
				compileIfNeeded(shaders[vertexShader].SourceCode, true, effect, *itr, pathToNxfxFolder, &pathToFile);
			}
			else
			{
				compileIfNeeded(shaders[vertexShader].SourceCode, true, effect, *itr, pathToNxfxFolder, nullptr);
			}
		}

		if (pixelShader >= 0)
		{
			if (shaders[pixelShader].SourceFile.empty() == false)
			{
				AbsoluteFilePath pathToFile;
				if (AbsoluteFilePath::IsAbsolutePath(shaders[pixelShader].SourceFile.c_str()))
					pathToFile = AbsoluteFilePath(shaders[pixelShader].SourceFile.c_str());
				else
					pathToFile = AbsoluteFilePath::ConvertRelativePath(pathToNxfxFolder, shaders[pixelShader].SourceFile.c_str());

				loadExternalCode(pathToFile, shaders[pixelShader].SourceCode);

				compileIfNeeded(shaders[pixelShader].SourceCode, false, effect, *itr, pathToNxfxFolder, &pathToFile);
			}
			else
			{
				compileIfNeeded(shaders[pixelShader].SourceCode, false, effect, *itr, pathToNxfxFolder, nullptr);
			}
		}
	}
}

void EffectCompiler::loadExternalCode(const AbsoluteFilePath& pathToFile, std::vector<unsigned char>& result)
{
	// maybe we loaded this shader earlier, so we can skip reading it again
	if (result.empty())
	{
		// load the source file
		std::ifstream input;
		input.open(pathToFile.GetPath().c_str(), std::ios::binary);

		if (input.is_open() == false)
			throw EffectToolException("Unable to find external shader", pathToFile.GetPath().c_str());

		// get the file size
		input.seekg(0, std::ios::end);
		auto size = input.tellg();
		input.seekg(0, std::ios::beg);

		// read the file
		result.resize(size);
		input.read((char*)result.data(), size);
		input.close();
	}
}

void EffectCompiler::compileIfNeeded(const std::vector<unsigned char>& source, bool vertex, EffectXml* effect, TechniqueMapXml& technique, const AbsoluteFilePath& pathToNxfxFolder, const AbsoluteFilePath* pathToFile)
{
	if ((technique.ProfileType & Profile::ANY_HLSL) == Profile::ANY_HLSL)
	{
		// this may be uncompiled HLSL. Check to see.
		if (isValidHLSLBytecodeHeader(source) == false)
		{
			std::vector<unsigned char> codeBuffer;

			// This doesn't seem to be HLSL bytecode. Must be source code. let's try compiling it...
			compileHLSL(technique.ProfileType, vertex, pathToNxfxFolder, pathToFile, source.data(), source.size(),
				vertex ? technique.VertexShaderEntryPoint.c_str() : technique.PixelShaderEntryPoint.c_str(), codeBuffer);

			// add a new shader with the bytecode of this particular compiled permutation
			std::string newShaderName = technique.Name + (vertex ? "VS" : "PS");
			effect->AddShader(newShaderName.c_str(), codeBuffer.data(), codeBuffer.size());

			// update the technique to reference the new shader
			if (vertex)
				technique.VertexShader = newShaderName;
			else
				technique.PixelShader = newShaderName;
		}
	}
}

bool EffectCompiler::isValidHLSLBytecodeHeader(const std::vector<unsigned char>& data)
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

void EffectCompiler::compileHLSL(Profile::eProfile profile, bool vertex, const AbsoluteFilePath& pathToNxfxFolder, const AbsoluteFilePath* pathToFile, const unsigned char* code, int codeLength, const char* entryPoint, std::vector<unsigned char>& result)
{
#ifdef _WIN32

	const char* target;
	if (vertex)
	{
		switch(profile)
		{
		case Profile::HLSL_2_0:
			target = "vs_2_0";
			break;
		case Profile::HLSL_2_a:
			target = "vs_2_a";
			break;
		case Profile::HLSL_2_b:
			target = "vs_2_a"; // TODO: is this the best way to handle this? Probably not.
			break;
		case Profile::HLSL_3_0:
			target = "vs_3_0";
			break;
		case Profile::HLSL_4_0:
			target = "vs_4_0";
			break;
		case Profile::HLSL_4_0_level_9_0:
			target = "vs_4_0_level_9_0";
			break;
		case Profile::HLSL_4_0_level_9_1:
			target = "vs_4_0_level_9_1";
			break;
		case Profile::HLSL_4_1:
			target = "vs_4_1";
			break;
		case Profile::HLSL_5_0:
			target = "vs_5_0";
			break;
		}
	}
	else
	{
		switch(profile)
		{
		case Profile::HLSL_2_0:
			target = "ps_2_0";
			break;
		case Profile::HLSL_2_a:
			target = "ps_2_a";
			break;
		case Profile::HLSL_2_b:
			target = "ps_2_a"; // TODO: is this the best way to handle this? Probably not.
			break;
		case Profile::HLSL_3_0:
			target = "ps_3_0";
			break;
		case Profile::HLSL_4_0:
			target = "ps_4_0";
			break;
		case Profile::HLSL_4_0_level_9_0:
			target = "ps_4_0_level_9_0";
			break;
		case Profile::HLSL_4_0_level_9_1:
			target = "ps_4_0_level_9_1";
			break;
		case Profile::HLSL_4_1:
			target = "ps_4_1";
			break;
		case Profile::HLSL_5_0:
			target = "ps_5_0";
			break;
		}
	}

	ID3DBlob* output, *errors;
	AbsoluteFilePath includerPath = (pathToFile == nullptr ? pathToNxfxFolder : *pathToFile);
	HLSLIncluder includer(includerPath, pathToFile == nullptr);
	const char* filePath = (pathToFile == nullptr ? nullptr : pathToFile->GetPath().c_str());

	auto r = D3DCompile(code, codeLength, filePath, nullptr, &includer, entryPoint, target, 0, 0, &output, &errors);

	if (r != S_OK)
	{
		throw EffectToolException("Error compiling HLSL shader", (char*)errors->GetBufferPointer());
	}

	result.resize(output->GetBufferSize());
	memcpy(result.data(), output->GetBufferPointer(), output->GetBufferSize());
#else
	throw EffectToolException("HLSL compiling is only available on Windows");
#endif
}