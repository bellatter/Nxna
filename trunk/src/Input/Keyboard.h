#ifndef INPUT_KEYBOARD_H
#define INPUT_KEYBOARD_H

#include <map>
#include "Keys.h"

namespace Nxna
{
namespace Input
{
	enum KeyState
	{
		KeyState_Up = 0,
		KeyState_Down
	};

	class Keyboard;

	struct KeyboardState
	{
		friend class Keyboard;

	private:
		unsigned int m_state1;

	public:
		KeyState GetState(Keys key);
		bool IsKeyDown(Keys key) { return GetState(key) == KeyState_Down; }
		bool IsKeyUp(Keys key) { return GetState(key) == KeyState_Up; }
	};

	class Keyboard
	{
		static KeyboardState m_current;

	public:
		static KeyboardState GetState() { return m_current; }

		static void InjectKeyDown(Keys key);
		static void InjectKeyUp(Keys key);
	};
}
}

#endif // INPUT_KEYBOARD_H