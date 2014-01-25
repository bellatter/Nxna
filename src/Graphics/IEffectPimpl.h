#ifndef NXNA_GRAPHICS_IEFFECTPIMPL_H
#define NXNA_GRAPHICS_IEFFECTPIMPL_H

#include <vector>
#include "Effect.h"

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

		static EffectParameter* CreateParameter(Effect* parent, EffectParameterType type, int numElements, void* handle, const char* name);
		static EffectTechnique* CreateTechnique(Effect* parent, const char* name, bool hidden);

	protected:
		int* GetRawValue(EffectParameter* parameter);
	};
}
}
}

#endif // NXNA_GRAPHICS_IEFFECTPIMPL_H
