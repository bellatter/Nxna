#ifndef EFFECTXML_H
#define EFFECTXML_H

#include <string>
#include <vector>

struct ElementType
{
	enum eElementType
	{
		Void = 0,
		Bool,
		Int32,
		Single,
		String,
		Texture,
		Texture1D,
		Texture2D,
		Texture3D,
		TextureCube
	};

	static eElementType Parse(const char* str);
};

struct Semantic
{
	enum eSemantic
	{
		Binormal = 1,
		BlendIndices = 2,
		BlendWeights = 3,
		Color = 4,
		Normal = 5,
		Position = 6,
		PointSize = 7,
		Tangent = 8,
		TexCoord = 9,
		Fog = 10,
		TessFactor = 11,
		VFace = 12,
		VPos = 13,
		Depth = 14
	};

	static eSemantic Parse(const char* str);
};

struct Profile
{
	enum eProfile
	{
		ANY_GLSL = 0x1000,
		ANY_GLSL_ES = ANY_GLSL | 0x0800,
		ANY_HLSL = 0x2000,

		GLSL_120 = ANY_GLSL | 1,
		GLSL_130 = ANY_GLSL | 2,
		GLSL_140 = ANY_GLSL | 3,
		GLSL_150 = ANY_GLSL | 4,
		GLSL_330 = ANY_GLSL | 5,
		GLSL_400 = ANY_GLSL | 6,
		GLSL_410 = ANY_GLSL | 7,
		GLSL_420 = ANY_GLSL | 8,
		GLSL_430 = ANY_GLSL | 9,
		GLSL_440 = ANY_GLSL | 10,

		GLSL_ES_100 = ANY_GLSL_ES | 1,
		GLSL_ES_300 = ANY_GLSL_ES | 2,

		HLSL_2_0 = ANY_HLSL | 1,
		HLSL_2_a = ANY_HLSL | 2,
		HLSL_2_b = ANY_HLSL | 3,
		HLSL_3_0 = ANY_HLSL | 4,
		HLSL_4_0 = ANY_HLSL | 5,
		HLSL_4_1 = ANY_HLSL | 6,
		HLSL_4_0_level_9_0 = ANY_HLSL | 7,
		HLSL_4_0_level_9_1 = ANY_HLSL | 8,
		HLSL_4_0_level_9_3 = ANY_HLSL | 9,
		HLSL_5_0 = ANY_HLSL | 10,

		PROFILE_UNKNOWN = 0xffff
	};

	static eProfile Parse(const char* str);
};

struct AttributeXml
{
	std::string Name;
	ElementType::eElementType Type;
	int NumElements;
	Semantic::eSemantic SemanticType;
	int Index;
};

struct TechniqueXml
{
	std::string Name;
	bool Hidden;
	std::vector<AttributeXml> Attributes;
};

struct ConstantXml
{
	std::string Name;
	ElementType::eElementType Type;
	int NumElements;
};

struct CBufferXml
{
	std::vector<ConstantXml> Constants;
};

struct ShaderXml
{
	std::string Name;
	std::string SourceFile;
	std::vector<unsigned char> SourceCode;
};

struct TechniqueMapXml
{
	std::string Name;
	Profile::eProfile ProfileType;
	std::string VertexShader;
	std::string PixelShader;
};

class EffectXml
{
	std::vector<TechniqueXml> m_techniques;
	std::vector<CBufferXml> m_cbuffers;
	std::vector<std::string> m_textures;
	std::vector<ShaderXml> m_shaders;
	std::vector<TechniqueMapXml> m_techniqueMaps;

public:
	EffectXml(const std::string& xml);

	std::vector<TechniqueXml>& GetTechniques() { return m_techniques; }
	const std::vector<TechniqueXml>& GetTechniques() const { return m_techniques; }
	std::vector<CBufferXml>& GetCBuffers() { return m_cbuffers; }
	const std::vector<CBufferXml>& GetCBuffers() const { return m_cbuffers; }
	std::vector<std::string>& GetTextures() { return m_textures; }
	const std::vector<std::string>& GetTextures() const { return m_textures; }
	std::vector<ShaderXml>& GetShaders() { return m_shaders; }
	const std::vector<ShaderXml>& GetShaders() const { return m_shaders; }
	std::vector<TechniqueMapXml>& GetTechniqueMaps() { return m_techniqueMaps; }
	const std::vector<TechniqueMapXml>& GetTechniqueMaps() const { return m_techniqueMaps; }

	int FindTechniqueByName(const char* name) const;
	int FindShaderByName(const char* name) const;

	void RemoveProfile(Profile::eProfile profile);
	void RemoveUnusedShaders();

	void LoadExternalShaders(const char* pathToNxfxFolder);
};

#endif // EFFECTXML_H
