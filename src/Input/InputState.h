#ifndef NXNA_INPUT_INPUTSTATE_H
#define NXNA_INPUT_INPUTSTATE_H

#include "Keys.h"

namespace Nxna
{
namespace Input
{
#define NXNA_BUTTON_STATE(data) (data & 0x80)
#define NXNA_BUTTON_TRANSITION_COUNT(data) (data & 0x7F)

	constexpr int ButtonStateReleased = 0;
	constexpr int ButtonStatePressed = 0x80;

	class InputState
	{
	public:

		int MouseX, MouseY, Wheel;
		int RelMouseX, RelMouseY, RelWheel;
		unsigned char MouseButtonData[5];

		unsigned char KeyboardKeysData[256];

		static const int KeyboardBufferLength = 8;
		unsigned int BufferedKeys[KeyboardBufferLength];
		unsigned char NumBufferedKeys;

		static void FrameReset(InputState* state)
		{
			for (int i = 0; i < 5; i++)
				state->MouseButtonData[i] &= 0x80;

			for (int i = 0; i < 256; i++)
				state->KeyboardKeysData[i] &= 0x80;

			state->RelMouseX = 0;
			state->RelMouseY = 0;
		}

		static void InjectMouseMove(InputState* state, int mx, int my)
		{
			int relX = mx - state->MouseX;
			int relY = my - state->MouseY;

			state->MouseX = mx;
			state->MouseY = my;
			state->RelMouseX = relX;
			state->RelMouseY = relY;
		}

		static void InjectMouseButtonEvent(InputState* state, bool isButtonDown, int button)
		{
			if (button < 0 || button >= 5) return;

			injectEvent(&state->MouseButtonData[button], isButtonDown);
		}

		static void InjectKeyEvent(InputState* state, bool isKeyDown, Key key)
		{
			unsigned int button = (unsigned int)key;
			injectEvent(&state->KeyboardKeysData[button], isKeyDown);
		}

		static bool IsKeyDown(InputState* state, Key key)
		{
			return (state->KeyboardKeysData[(int)key] & 0x80) == 0x80;
		}

		static int GetKeyTransitionCount(InputState* state, Key key)
		{
			return (state->KeyboardKeysData[(int)key] & 0x7f);
		}

		static bool IsMouseButtonDown(InputState* state, int button)
		{
			if (button < 0 || button >= 5) return false;

			return (state->MouseButtonData[button] & 0x80) == 0x80;
		}

	private:
		static inline void injectEvent(unsigned char* data, bool newState)
		{
			// update the transition count
			*data = (*data + 1) & 0x7f;

			// write the key state
			*data |= (newState ? 0x80 : 0);
		}
	};
}
}

#endif // NXNA_INPUT_INPUTSTATE_H
