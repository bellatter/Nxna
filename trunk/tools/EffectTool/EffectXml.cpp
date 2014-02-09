#include <cassert>
#include <fstream>
#include <sstream>
#include "EffectXml.h"
#include "EffectToolException.h"
#include "Utils.h"
#include "tinyxml2/tinyxml2.h"

int Stricmp(const char *p1, const char *p2)
{
  register unsigned char *s1 = (unsigned char *) p1;
  register unsigned char *s2 = (unsigned char *) p2;
  unsigned char c1, c2;
 
  do
  {
      c1 = (unsigned char) toupper((int)*s1++);
      c2 = (unsigned char) toupper((int)*s2++);
      if (c1 == '\0')
      {
            return c1 - c2;
      }
  }
  while (c1 == c2);
 
  return c1 - c2;
}

ElementType::eElementType ElementType::Parse(const char* str)
{
	if (Stricmp(str, "float") == 0)
		return ElementType::Single;

	assert("Unknown ElementType" && 0);

	// this is just to stop compiler warnings
	return ElementType::Single;
}

Semantic::eSemantic Semantic::Parse(const char* str)
{
	if (Stricmp(str, "binormal") == 0)
		return Binormal;
	if (Stricmp(str, "BlendIndices") == 0)
		return BlendIndices;
	if (Stricmp(str, "BlendWeights") == 0)
		return BlendWeights;
	if (Stricmp(str, "Color") == 0)
		return Color;
	if (Stricmp(str, "Normal") == 0)
		return Normal;
	if (Stricmp(str, "Position") == 0)
		return Position;
	if (Stricmp(str, "PointSize") == 0)
		return PointSize;
	if (Stricmp(str, "Tangent") == 0)
		return Tangent;
	if (Stricmp(str, "TexCoord") == 0)
		return TexCoord;
	if (Stricmp(str, "Fog") == 0)
		return Fog;
	if (Stricmp(str, "TessFactor") == 0)
		return TessFactor;
	if (Stricmp(str, "VFace") == 0)
		return VFace;
	if (Stricmp(str, "VPos") == 0)
		return VPos;
	if (Stricmp(str, "Depth") == 0)
		return Depth;

	assert("Unknown semantic" && 0);
	
	// this is just to stop compiler warnings
	return Position;
}

#define CHECK_ENUM(s,e) if (Stricmp(s, #e) == 0) return e;

Profile::eProfile Profile::Parse(const char* str)
{
	CHECK_ENUM(str, ANY_GLSL);
	CHECK_ENUM(str, ANY_GLSL_ES);
	CHECK_ENUM(str, ANY_HLSL);

	CHECK_ENUM(str, GLSL_120);
	CHECK_ENUM(str, GLSL_130);
	CHECK_ENUM(str, GLSL_140);
	CHECK_ENUM(str, GLSL_150);
	CHECK_ENUM(str, GLSL_330);
	CHECK_ENUM(str, GLSL_400);
	CHECK_ENUM(str, GLSL_410);
	CHECK_ENUM(str, GLSL_420);
	CHECK_ENUM(str, GLSL_430);
	CHECK_ENUM(str, GLSL_440);

	CHECK_ENUM(str, GLSL_ES_100);
	CHECK_ENUM(str, GLSL_ES_300);

	CHECK_ENUM(str, HLSL_2_0);
	CHECK_ENUM(str, HLSL_2_a);
	CHECK_ENUM(str, HLSL_2_b);
	CHECK_ENUM(str, HLSL_3_0);
	CHECK_ENUM(str, HLSL_4_0);
	CHECK_ENUM(str, HLSL_4_1);
	CHECK_ENUM(str, HLSL_4_0_level_9_0);
	CHECK_ENUM(str, HLSL_4_0_level_9_1);
	CHECK_ENUM(str, HLSL_4_0_level_9_3);
	CHECK_ENUM(str, HLSL_5_0);
	
	return PROFILE_UNKNOWN;
}

EffectXml::EffectXml(const std::string& xml)
{
	tinyxml2::XMLDocument doc;
	auto err = doc.Parse(xml.c_str());
	if (err != tinyxml2::XML_SUCCESS)
		throw std::exception("Error while parsing XML input");

	auto effect = doc.FirstChildElement("effect");
	auto techniques = effect->FirstChildElement("techniques");
	auto cbuffers = effect->FirstChildElement("cbuffers");
	auto textures = effect->FirstChildElement("textures");
	auto shaders = effect->FirstChildElement("shaders");
	auto shaderMap = effect->FirstChildElement("shaderMap");

	// parse the techniques
	if (techniques != nullptr)
	{
		auto technique = techniques->FirstChildElement("technique");
		while(technique != nullptr)
		{
			TechniqueXml techniqueXml;
			techniqueXml.Name = technique->Attribute("name");
			techniqueXml.Hidden = false;
			technique->QueryBoolAttribute("hidden", &techniqueXml.Hidden);

			auto attributes = technique->FirstChildElement("attributes");
			if (attributes != nullptr)
			{
				auto attribute = attributes->FirstChildElement("attribute");
				while(attribute != nullptr)
				{
					const char* name = attribute->Attribute("name");
					const char* type = attribute->Attribute("type");
					const char* semantic = attribute->Attribute("semantic");

					if (name == nullptr) throw EffectToolException("Attribute missing required attribute 'name'");
					if (type == nullptr) throw EffectToolException("Attribute missing required attribute 'type'");
					if (semantic == nullptr) throw EffectToolException("Attribute missing required attribute 'semantic'");

					AttributeXml attributeXml;
					attributeXml.Name = name;
					attributeXml.Type = ElementType::Parse(type);
					attributeXml.NumElements = attribute->IntAttribute("numElements");
					attributeXml.Semantic = Semantic::Parse(semantic);
					attributeXml.Index = attribute->IntAttribute("index");
					techniqueXml.Attributes.push_back(attributeXml);

					attribute = attribute->NextSiblingElement("attribute");
				}
			}

			m_techniques.push_back(techniqueXml);

			technique = technique->NextSiblingElement("technique");
		}
	}

	// parse the cbuffers
	if (cbuffers != nullptr)
	{
		auto cbuffer = cbuffers->FirstChildElement("cbuffer");
		while(cbuffer != nullptr)
		{
			CBufferXml cbufferXml;

			auto constant = cbuffer->FirstChildElement("constant");
			while(constant != nullptr)
			{
				ConstantXml constantXml;
				constantXml.Name = constant->Attribute("name");
				constantXml.Type = ElementType::Parse(constant->Attribute("type"));
				constantXml.NumElements = constant->IntAttribute("numElements");

				cbufferXml.Constants.push_back(constantXml);

				constant = constant->NextSiblingElement("constant");
			}

			m_cbuffers.push_back(cbufferXml);

			cbuffer = cbuffer->NextSiblingElement("cbuffer");
		}
	}

	// parse the textures
	if (textures != nullptr)
	{
		auto texture = textures->FirstChildElement("texture");
		while(texture != nullptr)
		{
			m_textures.push_back(texture->Attribute("name"));

			texture = texture->NextSiblingElement("texture");
		}
	}

	// parse the shaders
	if (shaders != nullptr)
	{
		auto shader = shaders->FirstChildElement("shader");
		while(shader != nullptr)
		{
			ShaderXml shaderXml;
			shaderXml.Name = shader->Attribute("name");
			if (shader->Attribute("sourceFile") != nullptr)
				shaderXml.SourceFile = shader->Attribute("sourceFile");
			if (shader->GetText() != nullptr)
			{
				const char* code = shader->GetText();
				size_t len = strlen(code);

				shaderXml.SourceCode.resize(len);
				memcpy(shaderXml.SourceCode.data(), code, len);
			}

			m_shaders.push_back(shaderXml);

			shader = shader->NextSiblingElement("shader");
		}
	}

	// parse the maps
	if (shaderMap != nullptr)
	{
		auto technique = shaderMap->FirstChildElement("technique");
		while(technique != nullptr)
		{
			TechniqueMapXml techniqueMapXml;
			techniqueMapXml.Name = technique->Attribute("name");
			techniqueMapXml.Profile = Profile::Parse(technique->Attribute("profile"));
			techniqueMapXml.VertexShader = technique->Attribute("vertexShader");
			techniqueMapXml.PixelShader = technique->Attribute("pixelShader");

			m_techniqueMaps.push_back(techniqueMapXml);

			technique = technique->NextSiblingElement("technique");
		}
	}
}

int EffectXml::FindTechniqueByName(const char* name) const
{
	int index = 0;
	for (auto itr = m_techniques.begin(); itr != m_techniques.end(); itr++)
	{
		if (Stricmp((*itr).Name.c_str(), name) == 0)
			return index;

		index++;
	}

	return -1;
}

int EffectXml::FindShaderByName(const char* name) const
{
	int index = 0;
	for (auto itr = m_shaders.begin(); itr != m_shaders.end(); itr++)
	{
		if (Stricmp((*itr).Name.c_str(), name) == 0)
			return index;

		index++;
	}

	return -1;
}

void EffectXml::RemoveProfile(Profile::eProfile profile)
{
	for (auto itr = m_techniqueMaps.begin(); itr != m_techniqueMaps.end(); )
	{
		if ((*itr).Profile == profile
			|| (profile == Profile::ANY_GLSL && (profile & Profile::ANY_GLSL) == Profile::ANY_GLSL)
			|| (profile == Profile::ANY_GLSL_ES && (profile & Profile::ANY_GLSL_ES) == Profile::ANY_GLSL_ES)
			|| (profile == Profile::ANY_HLSL && (profile & Profile::ANY_HLSL) == Profile::ANY_HLSL))
		{
			itr = m_techniqueMaps.erase(itr);
		}
		else
		{
			itr++;
		}
	}
}

void EffectXml::RemoveUnusedShaders()
{
	std::vector<bool> isShaderUsed(m_shaders.size());

	// mark used shaders as such
	for (auto itr = m_techniqueMaps.begin(); itr != m_techniqueMaps.end(); itr++)
	{
		int index = FindShaderByName((*itr).PixelShader.c_str());
		if (index >= 0)
			isShaderUsed[index] = true;

		index = FindShaderByName((*itr).VertexShader.c_str());
		if (index >= 0)
			isShaderUsed[index] = true;
	}

	// remove any shaders that aren't being used
	int i = 0;
	for (auto itr = m_shaders.begin(); itr != m_shaders.end(); itr++)
	{
		if (isShaderUsed[i] == false)
			m_shaders.erase(itr);

		i++;
	}
}

void EffectXml::LoadExternalShaders(const char* pathToNxfxFolder)
{
	for (auto itr = m_shaders.begin(); itr != m_shaders.end(); itr++)
	{
		if ((*itr).SourceFile.empty() == false)
		{
			// load the source file
			std::ifstream input;
			input.open((*itr).SourceFile.c_str(), std::ios::binary);

			if (input.is_open() == false)
			{
				// let's try resolving the file relative to the NXFX file
				char result[512];
				result[511] = 0;
				Utils::ResolvePathRelativeTo((*itr).SourceFile.c_str(), pathToNxfxFolder, result, 511);

				input.open(result, std::ios::binary);

				if (input.is_open() == false)
					throw EffectToolException("Unable to find external shader", (*itr).SourceFile.c_str());
			}

			// get the file size
			input.seekg(0, std::ios::end);
			auto size = input.tellg();
			input.seekg(0, std::ios::beg);

			(*itr).SourceCode.resize(size);
			input.read((char*)(*itr).SourceCode.data(), size);

			input.close();
		}
	}
}