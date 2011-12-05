#ifndef INPUT_BUTTONS_H
#define INPUT_BUTTONS_H

namespace Nxna
{
namespace Input
{
	enum Buttons
	{
		Buttons_DPadUp,
		Buttons_DPadDown,
		Buttons_DPadLeft,
		Buttons_DPadRight,
		Buttons_Start,
		Buttons_Back,
		Buttons_LeftStick,
		Buttons_RightStick,
		Buttons_LeftShoulder,
		Buttons_RightShoulder,
		Buttons_BigButton,
		Buttons_A,
		Buttons_B,
		Buttons_X,
		Buttons_Y

		// TODO: finish this
	};

	enum ButtonState
	{
		ButtonState_Released,
		ButtonState_Pressed
	};
}
}

#endif // INPUT_BUTTONS_H