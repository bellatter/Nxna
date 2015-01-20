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
		static Keys m_bufferedKeys[10];
		static int m_bufferedKeyNext;

	public:
		static KeyboardState GetState() { return m_current; }

		static void InjectKeyDown(Keys key);
		static void InjectKeyUp(Keys key);

		// The buffered stuff is not part of the official API, and
		// may change or be removed at any time.
		// The "best" way of allowing buffered input is probably to
		// let code hook into the message pump. But not all platforms
		// use a message pump. So whatever we do has to be flexible
		// enough to work on all platforms.
		static Keys* GetBufferedKeys() { return m_bufferedKeys; }
		static void ResetBufferedKeys() { m_bufferedKeys[0] = Keys::None; m_bufferedKeyNext = 0; }
	};
}
}

#endif // INPUT_KEYBOARD_H