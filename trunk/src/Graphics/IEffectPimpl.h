#ifndef NXNA_GRAPHICS_IEFFECTPIMPL_H
#define NXNA_GRAPHICS_IEFFECTPIMPL_H

#include <vector>
#include "Effect.h"
#include "ShaderProfile.h"
#include "Semantic.h"

namespace Nxna
{
namespace Graphics
{
namespace Pvt
{
	class IEffectPimpl
	{
	protected:
		Effect* m_parent;

	public:
		IEffectPimpl(Effect* parent) { m_parent = parent; }
		virtual ~IEffectPimpl() { }

		virtual void Apply(int techniqueIndex) = 0;

		virtual EffectParameter* GetParameter(const char* name) = 0;
		virtual EffectParameter* GetParameter(int index) = 0;
		virtual int GetNumParameters() = 0;

		virtual void AddConstantBuffer(bool vertex, bool pixel, int sizeInBytes, int numParameters) = 0;
		virtual EffectParameter* AddParameter(const char* name, EffectParameterType type, int numElements, int constantBufferIndex, int constantBufferConstantIndex, int constantBufferOffset) = 0;

		virtual EffectTechnique* CreateProgram(const char* name, bool hidden, const byte* vertexSource, int vertexSourceLength, const byte* pixelSource, int pixelSourceLength) = 0;
		virtual void AddAttributeToProgram(int programIndex, const char* name, EffectParameterType type, int numElements, Semantic semantic, int usageIndex) = 0;
		
		virtual int ScoreProfile(ShaderProfile profile) = 0;

		static EffectParameter* CreateParameter(Effect* parent, EffectParameterType type, int numElements, void* handle, const char* name);
	
	protected:
		int* GetRawValue(EffectParameter* parameter);

		EffectTechnique* CreateTechnique(const char* name, bool hidden);
	};
}
}
}

#endif // NXNA_GRAPHICS_IEFFECTPIMPL_H
