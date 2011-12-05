#ifndef INPUT_GAMEPAD_H
#define INPUT_GAMEPAD_H

#include "Buttons.h"
#include "../Vector2.h"

namespace Nxna
{
namespace Input
{
	struct GamePadButtons
	{
		ButtonState A;
		ButtonState B;
		ButtonState Back;
		ButtonState BigButton;
		ButtonState LeftShoulder;
		ButtonState LeftStick;
		ButtonState RightShoulder;
		ButtonState RightStick;
		ButtonState Start;
		ButtonState X;
		ButtonState Y;
	};

	struct GamePadDPad
	{
		ButtonState Down;
		ButtonState Left;
		ButtonState Right;
		ButtonState Up;
	};

	struct GamePadThumbSticks
	{
		Vector2 Left;
		Vector2 Right;
	};

	struct GamePadTriggers
	{
		float Left;
		float Right;
	};

	struct GamePadState
	{
		GamePadButtons Buttonz;
		GamePadDPad DPad;
		bool IsConnected;
		int PacketNumber;
		GamePadThumbSticks ThumbSticks;
		GamePadTriggers Triggers;

		bool IsButtonDown(Buttons button);
		bool IsButtonUp(Buttons button);
	};

	class GamePad
	{
		static GamePadState m_current;

	public:
		static GamePadState GetState() { return m_current; }
	};
}
}

#endif // INPUT_GAMEPAD_H