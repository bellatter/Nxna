#ifndef INPUT_KEYBOARD_H
#define INPUT_KEYBOARD_H

#include <map>
#include "Keys.h"

namespace Nxna
{
namespace Input
{
	NXNA_ENUM(KeyState)
		Up = 0,
		Down
	END_NXNA_ENUM(KeyState)

	class Keyboard;

	struct KeyboardState
	{
		friend class Keyboard;

	private:

		unsigned int m_states[8];

	public:
		KeyState GetState(Keys key) const;
		bool IsKeyDown(Keys key) const { return GetState(key) == KeyState::Down; }
		bool IsKeyUp(Keys key) const { return GetState(key) == KeyState::Up; }

		void GetPressedKeys(Keys* destination, int* destinationLength, int maxLength) const;
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