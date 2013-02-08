#ifndef INPUT_BUTTONS_H
#define INPUT_BUTTONS_H

#include "../NxnaConfig.h"

namespace Nxna
{
namespace Input
{
	NXNA_ENUM(Buttons)
		DPadUp,
		DPadDown,
		DPadLeft,
		DPadRight,
		Start,
		Back,
		LeftStick,
		RightStick,
		LeftShoulder,
		RightShoulder,
		BigButton,
		A,
		B,
		X,
		Y

		// TODO: finish this
	END_NXNA_ENUM(Buttons)

	NXNA_ENUM(ButtonState)
		Released,
		Pressed
	END_NXNA_ENUM(ButtonState)
}
}

#endif // INPUT_BUTTONS_H