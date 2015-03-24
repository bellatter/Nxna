#ifndef AUDIO_AUDIOEMITTER_H
#define AUDIO_AUDIOEMITTER_H

#include "../Vector3.h"

namespace Nxna
{
namespace Audio
{
	class SoundEffectInstance;

	class AudioEmitter
	{
		friend class SoundEffectInstance;

		Vector3 m_position;

	public:
		Vector3 GetPosition() const { return m_position; }
		void SetPosition(const Vector3& position) { m_position = position; }
	};
}
}

#endif // AUDIO_AUDIOEMITTER_H