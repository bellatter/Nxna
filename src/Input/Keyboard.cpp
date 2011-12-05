#include <cassert>
#include "Keyboard.h"

namespace Nxna
{
namespace Input
{
	KeyState KeyboardState::GetState(Keys key)
	{
		int keyVal = (int)key;
		assert(keyVal < 32);

		return (KeyState)(m_state1 & (1 << keyVal));
	}

	KeyboardState Keyboard::m_current;

	void Keyboard::InjectKeyDown(Keys key)
	{
		int keyVal = (int)key;
		assert(keyVal < 32);

		m_current.m_state1 |= 1 << keyVal;
	}

	void Keyboard::InjectKeyUp(Keys key)
	{
		int keyVal = (int)key;
		assert(keyVal < 32);

		m_current.m_state1 &= ~(1 << keyVal);
	}
}
}
