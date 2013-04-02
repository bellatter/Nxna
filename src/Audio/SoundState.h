#ifndef NXNA_AUDIO_SOUNDSTATE_H
#define NXNA_AUDIO_SOUNDSTATE_H

#include "../NxnaConfig.h"

namespace Nxna
{
namespace Audio
{
	NXNA_ENUM(SoundState)
		Paused,
		Playing,
		Stopped
	END_NXNA_ENUM(SoundState);
}
}

#endif // NXNA_AUDIO_SOUNDSTATE_H
