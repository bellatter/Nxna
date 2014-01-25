#include "IEffectPimpl.h"
#include "Effect.h"
#include "../Exception.h"

namespace Nxna
{
namespace Graphics
{
namespace Pvt
{
	EffectParameter* IEffectPimpl::CreateParameter(Effect* parent, EffectParameterType type, int numElements, void* handle, const char* name)
	{
		return new EffectParameter(parent, type, numElements, handle, name);
	}

	EffectTechnique* IEffectPimpl::CreateTechnique(Effect* parent, const char* name, bool hidden)
	{
		return parent->CreateTechnique(name, hidden);
	}

	int* IEffectPimpl::GetRawValue(EffectParameter* parameter)
	{
		return parameter->m_value;
	}
}
}
}