#include "Input.h"
#ifndef NXNA_PLATFORM_NACL
#include <SDL.h>
#else
#include <ppapi/cpp/input_event.h>
#include <ppapi/cpp/point.h>
#endif

namespace Nxna
{
namespace Platform
{
	Nxna::Input::MouseState Input::m_mouse;
	Nxna::Input::KeyboardState Input::m_keyboard;
	bool Input::m_quitReceived = false;

#ifndef NXNA_PLATFORM_NACL
	Nxna::Input::Keys convertSDLK(SDLKey sdlk)
	{
		if (sdlk == SDLK_BACKSPACE)
			return Nxna::Input::Keys_Back;
		if (sdlk == SDLK_RETURN)
			return Nxna::Input::Keys_Enter;

		return Nxna::Input::Keys_None;
	}
#endif

#ifdef NXNA_PLATFORM_APPLE_IOS
	extern bool g_isRetinaDisplay;
#endif

	void Input::Refresh()
	{
#ifndef NXNA_PLATFORM_NACL

		m_keyboard = Nxna::Input::Keyboard::GetState();
		m_mouse = Nxna::Input::Mouse::GetState();

		SDL_Event e;

		while(SDL_PollEvent(&e))
		{
			switch(e.type)
			{
			case SDL_QUIT:
				m_quitReceived = true;
				break;
			case SDL_MOUSEMOTION:
#ifdef NXNA_PLATFORM_APPLE_IOS
				if (g_isRetinaDisplay)
					Nxna::Input::Mouse::InjectMouseMove(e.motion.x * 2, e.motion.y * 2);
				else
#endif
				Nxna::Input::Mouse::InjectMouseMove(e.motion.x, e.motion.y);
				break;
			case SDL_MOUSEBUTTONDOWN:
				Nxna::Input::Mouse::InjectMouseButton(e.button.button, true);
				break;
			case SDL_MOUSEBUTTONUP:
				Nxna::Input::Mouse::InjectMouseButton(e.button.button, false);
				break;
			case SDL_KEYDOWN:
				Nxna::Input::Keyboard::InjectKeyDown(convertSDLK(e.key.keysym.sym));
				break;
			case SDL_KEYUP:
				Nxna::Input::Keyboard::InjectKeyUp(convertSDLK(e.key.keysym.sym));
				break;
			}
		}
#else
		// nothing to do with NaCl
#endif
	}

	
#ifdef NXNA_PLATFORM_NACL
	void Input::HandleEvent(const pp::InputEvent& inputEvent)
	{
		switch (inputEvent.GetType())
		{
		case PP_INPUTEVENT_TYPE_MOUSEDOWN:
			if (pp::MouseInputEvent(inputEvent).GetButton() == PP_INPUTEVENT_MOUSEBUTTON_LEFT)
				m_mouse.LeftButton = Nxna::Input::ButtonState_Pressed;
			break;
		case PP_INPUTEVENT_TYPE_MOUSEUP:
			if (pp::MouseInputEvent(inputEvent).GetButton() == PP_INPUTEVENT_MOUSEBUTTON_LEFT)
				m_mouse.LeftButton = Nxna::Input::ButtonState_Released;
			break;
		case PP_INPUTEVENT_TYPE_MOUSEMOVE:
			pp::Point p = pp::MouseInputEvent(inputEvent).GetPosition();
			m_mouse.X = p.x();
			m_mouse.Y = p.y();
		  break;
		/*case PP_INPUTEVENT_TYPE_KEYDOWN:
		  GotKeyEvent(pp::KeyboardInputEvent(event), "Down");
		  break;
		case PP_INPUTEVENT_TYPE_KEYUP:
		  GotKeyEvent(pp::KeyboardInputEvent(event), "Up");
		  break;
		case PP_INPUTEVENT_TYPE_CHAR:
		  GotKeyEvent(pp::KeyboardInputEvent(event), "Character");
		  break;*/
		}
	}
#endif
}
}