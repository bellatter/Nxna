#ifndef NXNA_SAMPLES_COMMON_H
#define NXNA_SAMPLES_COMMON_H

struct WindowInfo
{
#ifdef _WIN32
	HWND Window;
	bool HasGLContext;
#elif defined __linux__
	void* Display;
	unsigned int Window;
#else
	// TODO
#endif
};

bool CreateGameWindow(int width, int height, bool fullscreen, const char* title, WindowInfo* result);
void ShowGameWindow(WindowInfo window);
void DestroyGameWindow(WindowInfo window);

#ifdef _WIN32
bool CreateDirect3DDevice(WindowInfo* window, int width, int height, ID3D11Device** pdevice, ID3D11DeviceContext** pcontext, ID3D11RenderTargetView** prenderTargetView, IDXGISwapChain** pswapChain);
#endif

bool CreateOpenGLContext(WindowInfo* window);

void HandleMessages(WindowInfo window);

void Present(WindowInfo window);


#endif // NXNA_SAMPLES_COMMON_H
