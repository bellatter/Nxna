#ifndef NXNA_GRAPHICS_DIRECT3D11_HLSLEFFECT_H
#define NXNA_GRAPHICS_DIRECT3D11_HLSLEFFECT_H

#include <vector>
#include <string>
#include "../../NxnaConfig.h"
#include "../Effect.h"
#include "../IEffectPimpl.h"
#include "../VertexDeclaration.h"
#include "../ConstantBuffer.h"

NXNA_DISABLE_OVERRIDE_WARNING

namespace Nxna
{
namespace Graphics
{
namespace Direct3D11
{
	class Direct3D11Device;

	class HlslEffect : public Pvt::IEffectPimpl
	{
		friend class Direct3D11Device;

		struct GlslUniform
		{
			EffectParameter* Param;
			unsigned int Program;
			unsigned int Uniform;
		};

		struct HlslPermutation
		{
			void* D3DVertexShader;
			void* D3DPixelShader;
			unsigned int Hash;
			byte* VertexBytecode;
			int VertexBytecodeLength;
			//int* CBuffers;
			//int NumCBuffers;
		};

		struct strcmpop
		{
			bool operator()(const char* a, const char* b)
			{
				return strcmp(a, b) < 0;
			}
		};
		
		typedef std::map<const char*, EffectParameter*, strcmpop> ParamMap;
		ParamMap m_parameters;

		std::vector<HlslPermutation> m_permutations;
		std::vector<EffectParameter*> m_parameterList;
		std::vector<EffectParameter*> m_textureParams;
		
		static const int MAX_ATTRIB_SIZE = 256;
		static char m_attribNameBuffer[MAX_ATTRIB_SIZE];
		static int m_boundProgramIndex;
		Direct3D11Device* m_device;

	protected:
		std::vector<ConstantBuffer> m_cbuffers;

	public:
		HlslEffect(Direct3D11Device* device, Effect* parent);
		virtual ~HlslEffect();

		void AddPermutation(const char* name, bool hidden, const byte* vertexBytecode, int vertexBytecodeLength,
			const byte* pixelBytecode, int pixelBytecodeLength);
		void CreateDummyTechnique();

		virtual EffectParameter* GetParameter(const char* name) override
		{
			ParamMap::iterator itr = m_parameters.find(name);
			if (itr != m_parameters.end())
			{
				return (*itr).second;
			}
			
			return nullptr;
		}

		virtual EffectParameter* GetParameter(int index) override
		{
			return m_parameterList[index];
		}

		virtual int GetNumParameters() override
		{
			return m_parameterList.size();
		}

		unsigned int GetHash(int program);
		void GetBytecode(int program, byte** bytecode, int* length);

		void SetConstantBuffers();

		void ApplyProgram(int programIndex);

		EffectParameter* AddParameter(EffectParameterType type, int numElements, void* handle, const char* name);

		std::vector<ConstantBuffer>& GetConstantBuffers() { return m_cbuffers; }

	private:
		virtual void Apply(int techniqueIndex) override;

		int compile(const std::string& source, const char* defines, bool vertex);
		void processSource(std::string& source, bool vertex);
		std::string extractAttribInfo(const char* vertexShaderSource);
		void replaceVersionMacros(std::string& code);
		void replacePrecisionMacros(std::string& code);
		void fixParameters(std::string& code, bool isVertexShader);
		void replaceAll(std::string& original, const std::string& toRemove, const std::string& toPut);

		static const char* lastIndexNotSpace(const char* str, int startIndex)
        {
            for (int i = startIndex; i >= 0; i--)
            {
                if (str[i] != ' ')
                    return &str[i];
            }

            // not found :(
            return nullptr;
        }

        static const char* firstIndexNotSpace(const char* str)
        {
            for (size_t i = 0; i < strlen(str); i++)
            {
                if (str[i] != ' ')
                    return &str[i];
            }

            // not found :(
            return nullptr;
        }

		static const char* lastIndexOf(const char* str, char chr, int start)
		{
			for (int i = start; i >= 0; i--)
			{
				if (str[i] == chr)
					return &str[i];
			}

			return nullptr;
		}

		static const char* strchrAny(const char* str, const char* list, int length)
		{
			for (size_t i = 0; i < strlen(str); i++)
			{
				for (int j = 0; j < length; j++)
				{
					if (str[i] == list[j])
						return &str[i];
				}
			}

			return nullptr;
		}

	};
}
}
}

NXNA_ENABLE_OVERRIDE_WARNING

#endif // NXNA_GRAPHICS_DIRECT3D11_HLSLEFFECT_H