#include "Effect.h"
#include "GraphicsDevice.h"
#include "IEffectPimpl.h"
#include "../Content/XnbReader.h"
#include "../Content/FileStream.h"

namespace Nxna
{
namespace Graphics
{
	NXNA_ENUM(Profile)
		GLSL_120 = 1,
		GLSL_130 = 2,
		GLSL_140 = 3,
		GLSL_150 = 4,
		GLSL_330 = 5,
		GLSL_400 = 6,
		GLSL_410 = 7,
		GLSL_420 = 8,
		GLSL_430 = 9,
		GLSL_440 = 10,

		GLSL_ES_100 = 11,
		GLSL_ES_300 = 12,

		HLSL_2_0 = 13,
		HLSL_2_a = 14,
		HLSL_2_b = 15,
		HLSL_3_0 = 16,
		HLSL_4_0 = 17,
		HLSL_4_1 = 18,
		HLSL_4_0_level_9_0 = 19,
		HLSL_4_0_level_9_1 = 20,
		HLSL_4_0_level_9_3 = 21,
		HLSL_5_0 = 22
	END_NXNA_ENUM(Profile)

	struct Attrib
	{
		std::string Name;
		EffectParameterType Type;
		int NumElements;
		Semantic Sem;
		int Index;
	};

	struct Tech
	{
		std::string Name;
		bool Hidden;
		std::vector<Attrib> Attributes;
	};

	struct CBufferElement
	{
		std::string Name;
		EffectParameterType Type;
		int NumElements;
	};

	struct CBuffer
	{
		std::vector<CBufferElement> Elements;
	};

	struct ProfileShaderMap
	{
		int ProfileScore;
		int TechniqueIndex;
		int VertexShaderIndex;
		int PixelShaderIndex;
	};

	Effect::Effect(GraphicsDevice* device, const byte* effectCode, int effectCodeLength)
	{
		m_device = device;
		m_currentTechniqueIndex = -1;
		m_pimpl = device->CreateEffectPimpl(this);

		Content::MemoryStream code(effectCode, effectCodeLength);
		int magic = code.ReadInt32();
		byte version = code.ReadByte();
		
		short numTechniques = code.ReadInt16();
		short numCbuffers = code.ReadInt16();
		short numTextures = code.ReadInt16();
		short numShaders = code.ReadInt16();
		short numTechMaps = code.ReadInt16();

		// read the techniques
		std::vector<Tech> techniques;
		for (short i = 0; i < numTechniques; i++)
		{
			Tech t;
			byte nameLen = code.ReadByte();
			t.Name.resize(nameLen + 1); // does resize() include the null terminator?
			code.Read((byte*)t.Name.c_str(), nameLen);
			t.Hidden = code.ReadByte() != 0;

			byte numAttributes = code.ReadByte();
			for (byte j = 0; j < numAttributes; j++)
			{
				Attrib a;
				nameLen = code.ReadByte();
				a.Name.resize(nameLen + 1);
				code.Read((byte*)a.Name.c_str(), nameLen);

				a.Type = (EffectParameterType)code.ReadByte();
				a.NumElements = code.ReadByte();
				a.Sem = (Semantic)code.ReadByte();
				a.Index = code.ReadByte();

				t.Attributes.push_back(a);
			}

			techniques.push_back(t);
		}

		std::vector<CBuffer> cbuffers;

		// read the cbuffers
		for (short i = 0; i < numCbuffers; i++)
		{
			CBuffer b;

			byte numConstants = code.ReadByte();
			for (byte j = 0; j < numConstants; j++)
			{
				CBufferElement e;
				byte nameLen = code.ReadByte();
				e.Name.resize(nameLen + 1);
				code.Read((byte*)e.Name.c_str(), nameLen);
				e.Type = (EffectParameterType)code.ReadByte();
				e.NumElements = code.ReadByte();

				b.Elements.push_back(e);
			}

			cbuffers.push_back(b);
		}

		// read the textures
		std::vector<std::string> textures;
		for (short i = 0; i < numTextures; i++)
		{
			std::string texture;
			byte nameLen = code.ReadByte();
			texture.resize(nameLen + 1);
			code.Read((byte*)texture.c_str(), nameLen);

			textures.push_back(texture);
		}

		// read the shaders
		std::vector<int> shaderOffsets;
		for (short i = 0; i < numShaders; i++)
		{
			shaderOffsets.push_back(code.Position());

			int size = code.ReadInt32();
			code.Seek(size, Content::SeekOrigin::Current);
		}

		std::vector<ProfileShaderMap> profileShaderMap;
		for (short i = 0; i < numTechMaps; i++)
		{
			ProfileShaderMap psm;
				
			short profile = code.ReadInt16();
			psm.ProfileScore = m_pimpl->ScoreProfile((ShaderProfile)profile);
			psm.TechniqueIndex = code.ReadInt16();
			psm.VertexShaderIndex = code.ReadInt16();
			psm.PixelShaderIndex = code.ReadInt16();

			profileShaderMap.push_back(psm);
		}


		// create parameters and cbuffers
		for (int i = 0; i < cbuffers.size(); i++)
		{
			m_pimpl->AddConstantBuffer(true, true, 128, cbuffers[i].Elements.size());

			int offset = 0;
			for (int j = 0; j < cbuffers[i].Elements.size(); j++)
			{
				m_pimpl->AddParameter(cbuffers[i].Elements[j].Name.c_str(), EffectParameterType::Single, cbuffers[i].Elements[j].NumElements, i, j, offset);
				offset += cbuffers[i].Elements[j].NumElements * sizeof(float);
			}
		}

		for (int i = 0; i < textures.size(); i++)
		{
			m_pimpl->AddParameter(textures[i].c_str(), EffectParameterType::Texture2D, 1, 0, 0, 0);
		}

		// create a program for each technique
		for (int i = 0; i < techniques.size(); i++)
		{
			// find best profile
			int bestProfileIndex = -1;
			int bestProfileScore = -1;
			for (int j = 0; j < profileShaderMap.size(); j++)
			{
				if (profileShaderMap[j].TechniqueIndex == i && profileShaderMap[j].ProfileScore > bestProfileScore)
				{
					bestProfileIndex = j;
					bestProfileScore = profileShaderMap[j].ProfileScore;
				}
			}

			if (bestProfileIndex < 0)
			{
				m_pimpl->CreateProgram(techniques[i].Name.c_str(), false, nullptr, 0, nullptr, 0);
				continue;
			}

			// go back and read the shader source for the one with the best size
			code.Seek(shaderOffsets[profileShaderMap[bestProfileIndex].VertexShaderIndex], Content::SeekOrigin::Begin);
			int vertexCodeLength = code.ReadInt32();
			std::vector<byte> vertexCodeBuffer;
			vertexCodeBuffer.resize(vertexCodeLength + 1);
			code.Read(vertexCodeBuffer.data(), vertexCodeLength);

			code.Seek(shaderOffsets[profileShaderMap[bestProfileIndex].PixelShaderIndex], Content::SeekOrigin::Begin);
			int pixelCodeLength = code.ReadInt32();
			std::vector<byte> pixelCodeBuffer;
			pixelCodeBuffer.resize(pixelCodeLength + 1);
			code.Read(pixelCodeBuffer.data(), pixelCodeLength);

			m_pimpl->CreateProgram(techniques[i].Name.c_str(), false, vertexCodeBuffer.data(), vertexCodeLength, pixelCodeBuffer.data(), pixelCodeLength);

			for (int j = 0; j < techniques[i].Attributes.size(); j++)
			{
				m_pimpl->AddAttributeToProgram(i, techniques[i].Attributes[j].Name.c_str(), techniques[i].Attributes[j].Type, techniques[i].Attributes[j].NumElements, techniques[i].Attributes[j].Sem, techniques[i].Attributes[j].Index);
			}
		}
	}

	Effect::Effect(GraphicsDevice* device)
	{
		m_device = device;
		m_currentTechniqueIndex = -1;
		m_pimpl = device->CreateEffectPimpl(this);
	}

	Effect::Effect(GraphicsDevice* device, Pvt::IEffectPimpl* pimpl)
	{
		m_device = device;
		m_currentTechniqueIndex = -1;
		m_pimpl = pimpl;
	}

	Effect::~Effect()
	{
		for (auto itr = m_techniques.begin(); itr != m_techniques.end(); itr++)
		{
			EffectTechnique* technique = (*itr);
			delete technique;
		}

		for (auto itr = m_hiddenTechniques.begin(); itr != m_hiddenTechniques.end(); itr++)
		{
			EffectTechnique* technique = (*itr);
			delete technique;
		}
	}

	EffectParameter* Effect::GetParameter(int index)
	{
		return m_pimpl->GetParameter(index);
	}

	EffectParameter* Effect::GetParameter(const char* name)
	{
		return m_pimpl->GetParameter(name);
	}

	int Effect::GetNumParameters()
	{
		return m_pimpl->GetNumParameters();
	}

	EffectTechnique* Effect::GetTechnique(int index)
	{
		if (index < 0 || index >= m_techniques.size())
			throw ArgumentException("index");

		return m_techniques[index];
	}

	EffectTechnique* Effect::GetTechnique(const char* name)
	{
		for (auto itr = m_techniques.begin(); itr != m_techniques.end(); itr++)
		{
			if ((*itr)->Name == name)
				return (*itr);
		}

		// apparently no technique by that name exists
		throw ArgumentException(name);
	}

	int Effect::GetNumTechniques()
	{
		return m_techniques.size();
	}

	void Effect::SetCurrentTechnique(EffectTechnique* technique)
	{
		if (technique->m_parentEffect != this || technique->m_hidden == true)
			throw InvalidOperationException("The technique does not belong to this effect.");

		m_currentTechniqueIndex = technique->m_index;
	}

	EffectTechnique* Effect::GetCurrentTechnique()
	{
		if (m_currentTechniqueIndex < 0)
			return nullptr;

		return m_techniques[m_currentTechniqueIndex];
	}

	void Effect::OnApply()
	{
		if (m_currentTechniqueIndex < 0)
			throw InvalidOperationException("The effect has no current technique set");

		m_pimpl->Apply(m_currentTechniqueIndex);
	}

	EffectTechnique* Effect::CreateTechnique(const char* name, bool hidden)
	{
		EffectTechnique* technique;

		if (hidden)
		{
			technique = new EffectTechnique(this, name, m_hiddenTechniques.size(), true);
			m_hiddenTechniques.push_back(technique);
		}
		else
		{
			technique = new EffectTechnique(this, name, m_techniques.size(), false);
			m_techniques.push_back(technique);

			if (m_techniques.size() == 1)
				m_currentTechniqueIndex = 0;
		}

		return technique;
	}

	void EffectTechnique::Apply()
	{
		if (m_parentEffect->GetCurrentTechnique() != this)
			throw InvalidOperationException("The technique you are trying to Apply() is not the current technique.");

		m_parentEffect->OnApply();
	}
}
}