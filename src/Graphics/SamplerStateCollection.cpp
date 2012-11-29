#include "SamplerStateCollection.h"
#include "../Exception.h"

namespace Nxna
{
namespace Graphics
{
	const SamplerState* SamplerStateCollection::Get(int index)
	{
		if (index < 0 || index >= NUM_SAMPLER_STATES)
			throw ArgumentException("index");

		if ((m_nullFlags & (1 << index)) == 0)
			return &m_samplerStates[index];

		return nullptr;
	}

	void SamplerStateCollection::Set(int index, const SamplerState* state)
	{
		if (index < 0 || index >= NUM_SAMPLER_STATES)
			throw ArgumentException("index");

		if (state == nullptr)
			m_nullFlags |= (1 << index);
		else
		{
			m_samplerStates[index] = *state;
			m_nullFlags &= ~(1 << index);
		}

		m_dirtyFlags |= (1 << index);
	}
}
}