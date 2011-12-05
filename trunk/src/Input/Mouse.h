#ifndef INPUT_MOUSE_H
#define INPUT_MOUSE_H

#include "Buttons.h"

namespace Nxna
{
namespace Input
{
	struct MouseState
	{
		int X, Y;
		ButtonState LeftButton;
		ButtonState MiddleButton;
		ButtonState RightButton;
	};

	class Mouse
	{
		static MouseState _current;

	public:

		static MouseState GetState() { return _current; }

		static void InjectMouseMove(int mx, int my);
		static void InjectMouseButton(int button, bool down);
	};
}
}

#endif // INPUT_MOUSE_H
