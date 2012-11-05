#include <cassert>
#include "WindowsOpenGlWindow.h"
#include "WindowsGame.h"
#include "../../Graphics/OpenGl/OpenGlDevice.h"
#include "../../Input/Touch/TouchPanel.h"
#include "WindowsCommon.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

typedef HGLRC(APIENTRY* PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC hDC, HGLRC hShareContext, const int * attribList);

namespace Nxna
{
namespace Platform
{
namespace Windows
{
	WindowsOpenGlWindow::WindowsOpenGlWindow(Nxna::Game* game)
		: Nxna::GraphicsDeviceManager(game)
	{
	}

	Nxna::Graphics::GraphicsDevice* WindowsOpenGlWindow::CreateDevice()
	{
		m_device = new Nxna::Graphics::OpenGl::OpenGlDevice();

		return m_device;
	}

	void WindowsOpenGlWindow::BeginDraw()
	{
	}

	void WindowsOpenGlWindow::EndDraw()
	{
		SwapBuffers((HDC)m_hdc);
	}

	void WindowsOpenGlWindow::ApplyChanges()
	{
	}

	void WindowsOpenGlWindow::ShowWindow()
	{
	}

	void WindowsOpenGlWindow::DestroyWindow()
	{
	}

	void WindowsOpenGlWindow::SetScreenSize(int width, int height, bool fullscreen)
	{
		assert(m_device != nullptr);

		m_window = CreateGameWindow(width, height, fullscreen);

        PreferredBackBufferWidth(width);
        PreferredBackBufferHeight(height);

		// now create the OpenGL context
		static PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
		{
			sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
			1,											// Version Number
			PFD_DRAW_TO_WINDOW |						// Format Must Support Window
			PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
			PFD_DOUBLEBUFFER,							// Must Support Double Buffering
			PFD_TYPE_RGBA,								// Request An RGBA Format
			32,						  					// Select Our Color Depth
			0, 0, 0, 0, 0, 0,							// Color Bits Ignored
			8,											// No Alpha Buffer
			0,											// Shift Bit Ignored
			0,											// No Accumulation Buffer
			0, 0, 0, 0,									// Accumulation Bits Ignored
			24,											// 16Bit Z-Buffer (Depth Buffer)  
			8,											// No Stencil Buffer
			0,											// No Auxiliary Buffer
			PFD_MAIN_PLANE,								// Main Drawing Layer
			0,											// Reserved
			0, 0, 0										// Layer Masks Ignored
		};

		m_hdc = GetDC((HWND)m_window);
		int pixelFormat = ChoosePixelFormat((HDC)m_hdc, &pfd);
		SetPixelFormat((HDC)m_hdc, pixelFormat, &pfd);
		HGLRC context = wglCreateContext((HDC)m_hdc);
		wglMakeCurrent((HDC)m_hdc, context);

		PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB =
			(PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");

#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB 0x00000002
#define WGL_CONTEXT_MAJOR_VERSION_ARB  0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB  0x2092
#define WGL_CONTEXT_FLAGS_ARB          0x2094

		int attribList[] =
		{
		   WGL_CONTEXT_MAJOR_VERSION_ARB, 2,
		   WGL_CONTEXT_MINOR_VERSION_ARB, 0,
		 //  WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
		 //  WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
		   0
		};

		HGLRC newContext = wglCreateContextAttribsARB((HDC)m_hdc, 0, attribList);
		if (wglMakeCurrent((HDC)m_hdc, newContext) == FALSE)
			throw Exception("zomg");
		wglDeleteContext((HGLRC)context);

		::ShowWindow((HWND)m_window, SW_SHOW);
		SetForegroundWindow((HWND)m_window);
		SetFocus((HWND)m_window);

		static_cast<Nxna::Graphics::OpenGl::OpenGlDevice*>(m_device)->OnContextCreated();
		static_cast<Nxna::Graphics::OpenGl::OpenGlDevice*>(m_device)->UpdateScreenSize(PreferredBackBufferWidth(), PreferredBackBufferHeight());

		m_device->SetViewport(Nxna::Graphics::Viewport(0, 0, PreferredBackBufferWidth(), PreferredBackBufferHeight()));

		// tell the touch panel the display size
		Input::Touch::TouchPanel::SetDisplayWidth(width);
		Input::Touch::TouchPanel::SetDisplayHeight(height);
	}

	
}
}
}