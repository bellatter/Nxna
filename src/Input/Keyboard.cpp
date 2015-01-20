#include <cassert>
#include "Keyboard.h"

namespace Nxna
{
namespace Input
{
	KeyState KeyboardState::GetState(Keys key) const
	{
		unsigned int stateIndex = (int)key >> 5;
		if (stateIndex >= 8) return KeyState::Up;

		unsigned int blah = 1 << (int)key;
		if ((blah & m_states[stateIndex]) == 0)
			return KeyState::Up;

		return KeyState::Down;
	}

	void KeyboardState::GetPressedKeys(Keys* destination, int* destinationLength, int maxLength) const
	{
		int len = 0;

		for (int i = 0; i < 256; i++)
		{
			if (len < maxLength && GetState((Keys)i) == KeyState::Down)
				destination[len++] = (Keys)i;
		}

		if (destinationLength != nullptr)
			*destinationLength = len;
	}

	KeyboardState Keyboard::m_current;
	Keys Keyboard::m_bufferedKeys[];
	int Keyboard::m_bufferedKeyNext = 0;

	void Keyboard::InjectKeyDown(Keys key)
	{
		unsigned int stateIndex = (unsigned int)key >> 5;
		m_current.m_states[stateIndex] |= ((1 << (unsigned int)key) & 0xffffffffff);

		if (m_bufferedKeyNext < 10)
		{
			m_bufferedKeys[m_bufferedKeyNext++] = key;
			m_bufferedKeys[m_bufferedKeyNext] = Keys::None;
		}
	}

	void Keyboard::InjectKeyUp(Keys key)
	{
		unsigned int stateIndex = (unsigned int)key >> 5;
		m_current.m_states[stateIndex] &= ~((1 << (unsigned int)key) & 0xffffffffff);
	}
}
}
