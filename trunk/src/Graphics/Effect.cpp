#include "Effect.h"

namespace Nxna
{
namespace Graphics
{
	int* Effect::GetRawValue(EffectParameter* parameter)
	{
		return parameter->m_value;
	}
}
}