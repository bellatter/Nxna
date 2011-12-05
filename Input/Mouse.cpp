#include "Mouse.h"

namespace Nxna
{
namespace Input
{
	MouseState Mouse::_current;

	void Mouse::InjectMouseMove(int mx, int my)
	{
		_current.X = mx;
		_current.Y = my;
	}

	void Mouse::InjectMouseButton(int button, bool down)
	{
		ButtonState state = down ? ButtonState_Pressed : ButtonState_Released;

		if (button == 1)
			_current.LeftButton = state;
		else if (button == 2)
			_current.MiddleButton = state;
		else if (button == 3)
			_current.RightButton = state;
	}
}
}
