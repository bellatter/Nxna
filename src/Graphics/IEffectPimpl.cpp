#include "IEffectPimpl.h"
#include "Effect.h"

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

	int* IEffectPimpl::GetRawValue(EffectParameter* parameter)
	{
		return parameter->m_value;
	}
}
}
}