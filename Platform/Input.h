#ifndef NXNA_PLATFORM_INPUT_H
#define NXNA_PLATFORM_INPUT_H

#include "PlatformDefs.h"

#ifdef NXNA_PLATFORM_NACL
#include <ppapi/cpp/input_event.h>
#endif

#include "../Input/Mouse.h"
#include "../Input/Keyboard.h"

namespace Nxna
{
namespace Platform
{
	class Input
	{
		static Nxna::Input::MouseState m_mouse;
		static Nxna::Input::KeyboardState m_keyboard;
		static bool m_quitReceived;

	public:
		static void Refresh();
		static Nxna::Input::MouseState GetMouseState();
		static Nxna::Input::KeyboardState GetKeyboardState();
		static bool WasQuitReceived() { return m_quitReceived; }

#ifdef NXNA_PLATFORM_NACL
		static void HandleEvent(const pp::InputEvent& inputEvent);
#endif
	};
}
}

#endif // NXNA_PLATFORM_INPUT_H