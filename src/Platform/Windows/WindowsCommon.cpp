#include "WindowsCommon.h"
#include "../../Exception.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

namespace Nxna
{
namespace Platform
{
namespace Windows
{
	void* CreateGameWindow(int width, int height, bool fullscreen)
	{
		HINSTANCE instance = GetModuleHandle(nullptr);

		WNDCLASSEX wc;
		wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wc.lpfnWndProc = WndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = instance;
		wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
		wc.hIconSm = wc.hIcon;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = nullptr;
		wc.lpszMenuName = NULL;
		wc.lpszClassName = "NXNA";
		wc.cbSize = sizeof(WNDCLASSEX);

		if (RegisterClassEx(&wc) == 0)
			throw Exception("Unable to register window class", __FILE__, __LINE__);

		int screenPosX = 0, screenPosY = 0;
		if (fullscreen)
		{
			DEVMODE screen;
			memset(&screen, 0, sizeof(screen));
			screen.dmSize = sizeof(screen);
			screen.dmPelsWidth = width;
			screen.dmPelsHeight = height;
			screen.dmBitsPerPel = 32;
			screen.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

			ChangeDisplaySettings(&screen, CDS_FULLSCREEN);
		}
		else
		{
			screenPosX = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
			screenPosY = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;
		}

		DWORD exStyle = WS_EX_APPWINDOW | (fullscreen ? WS_EX_WINDOWEDGE : 0);
		DWORD style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | (fullscreen ? WS_POPUP : WS_OVERLAPPEDWINDOW);

		RECT windowRect;
		windowRect.left = 0;
		windowRect.top = 0;
		windowRect.right = width;
		windowRect.bottom = height;
		AdjustWindowRectEx(&windowRect, style, false, exStyle);

		HWND window = CreateWindowEx(exStyle, "NXNA", "NXNA", style,
			screenPosX, screenPosY,
			windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
			nullptr, nullptr, instance, nullptr);
		if (window == nullptr)
			throw Exception("Unable to create window", __FILE__, __LINE__);

		return window;
	}
}
}
}
