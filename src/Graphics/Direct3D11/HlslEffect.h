#ifndef NXNA_GRAPHICS_DIRECT3D11_HLSLEFFECT_H
#define NXNA_GRAPHICS_DIRECT3D11_HLSLEFFECT_H

#include <vector>
#include <string>
#include "../../NxnaConfig.h"
#include "../Effect.h"
#include "../VertexDeclaration.h"

namespace Nxna
{
namespace Graphics
{
namespace Direct3D11
{
	class Direct3D11Device;

	struct GlslAttribute
	{
		std::string Name;
		int Handle;
		int GlHandle;
		int Index;
		Nxna::Graphics::VertexElementUsage Usage;
	};

	class HlslEffect : virtual public Effect
	{
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
		};

		enum CacheType
		{
			CacheType_Float,
			CacheType_Int
		};

		struct ParamCache
		{
			CacheType CachedValueType;
			int CachedValueSize;
			int CachedValue[4 * 4];
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
		std::vector<GlslAttribute> m_attributes;
		std::vector<EffectParameter*> m_parameterList;
		std::vector<EffectParameter*> m_textureParams;
		std::vector<ParamCache> m_cache;

		static const int MAX_ATTRIB_SIZE = 256;
		static char m_attribNameBuffer[MAX_ATTRIB_SIZE];
		static int m_boundProgramIndex;

	public:
		HlslEffect(Direct3D11Device* device);
		virtual ~HlslEffect();

		void AddPermutation(const byte* vertexBytecode, int vertexBytecodeLength, const byte* pixelBytecode, int pixelBytecodeLength);

		virtual EffectParameter* AddParameter(EffectParameterType type, void* handle, const char* name) override { return nullptr; };

		virtual void Apply() override { }

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

		const GlslAttribute* GetAttribute(VertexElementUsage usage, int index);

	protected:

		virtual void SetParameter(EffectParameter* param, Texture2D* texture) override { }
		virtual void SetParameter(EffectParameter* param, const Vector4& value) override { }
		virtual void SetParameter(EffectParameter* param, float matrix4x4[]) override { }
		virtual void SetParameter(EffectParameter* param, const Matrix& matrix) override { }

		void ApplyProgram(int programIndex);

	private:
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
            for (int i = 0; i < strlen(str); i++)
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
			for (int i = 0; i < strlen(str); i++)
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

#endif // NXNA_GRAPHICS_DIRECT3D11_HLSLEFFECT_H