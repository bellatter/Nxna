#ifndef NXNA_GRAPHICS_SAMPLERSTATECOLLECTION_H
#define NXNA_GRAPHICS_SAMPLERSTATECOLLECTION_H

#include "../NxnaConfig.h"
#include "SamplerState.h"

namespace Nxna
{
namespace Graphics
{
	class GraphicsDevice;

	class SamplerStateCollection
	{
		friend class GraphicsDevice;

		unsigned int m_dirtyFlags;
		unsigned int m_nullFlags;
		
		static const int NUM_SAMPLER_STATES = 8;
		SamplerState m_samplerStates[NUM_SAMPLER_STATES];

	public:
		SamplerStateCollection()
		{
			m_dirtyFlags = 0;
			m_nullFlags = 0;
		}

		// TODO: find a way to use the [] operator.
		// The problem is it works quite a bit different from the []
		// operator in C#. We need to store the SamplerStates locally
		// (and not just pointers) so that we don't have to require that
		// the rest of the app maintains those sampler states that were
		// given to us, but we have no way of seeing the value the user
		// wants to store, so we *can't* store them locally.
		const SamplerState* Get(int index);
		void Set(int index, const SamplerState* state);

		void MakeDirty() { m_dirtyFlags = 0xffffffff; }
		void MakeClean() { m_dirtyFlags = 0; }

		bool IsDirty(int index) { return (m_dirtyFlags & (1 << index)) != 0; }
		int GetCount() { return NUM_SAMPLER_STATES; }

	protected:

		// only the GraphicsDevice should call this once all shaders textures have been bound
		void SetSamplers();
	};
}
}

#endif // NXNA_GRAPHICS_SAMPLERSTATECOLLECTION_H