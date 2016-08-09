
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
typedef HGLRC(APIENTRY* PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC hDC, HGLRC hShareContext, const int * attribList);

bool CreateGameWindow(int width, int height, bool fullscreen, const char* title, WindowInfo* result)
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
		return false;

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
	DWORD style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | (fullscreen ? WS_POPUP : (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX));

	RECT windowRect;
	windowRect.left = 0;
	windowRect.top = 0;
	windowRect.right = width;
	windowRect.bottom = height;
	AdjustWindowRectEx(&windowRect, style, false, exStyle);

	HWND window = CreateWindowEx(exStyle, "NXNA", title, style,
		screenPosX, screenPosY,
		windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
		nullptr, nullptr, instance, nullptr);
	if (window == nullptr)
		return false;

	result->HasGLContext = false;
	result->Window = window;
	return true;
}

void ShowGameWindow(WindowInfo window)
{
	::ShowWindow(window.Window, SW_SHOW);
	SetForegroundWindow(window.Window);
	SetFocus(window.Window);
}

void DestroyGameWindow(WindowInfo window)
{
	// nothing
}

bool CreateOpenGLContext(WindowInfo* window)
{
	// now create the OpenGL context
	static PIXELFORMATDESCRIPTOR pfd =				// pfd Tells Windows How We Want Things To Be
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

	auto hdc = GetDC(window->Window);
	int pixelFormat = ChoosePixelFormat(hdc, &pfd);
	SetPixelFormat(hdc, pixelFormat, &pfd);
	HGLRC context = wglCreateContext(hdc);
	wglMakeCurrent(hdc, context);

	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB =
		(PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");

#define WGL_CONTEXT_DEBUG_BIT_ARB 0x0001
#define WGL_CONTEXT_MAJOR_VERSION_ARB  0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB  0x2092
#define WGL_CONTEXT_FLAGS_ARB          0x2094
#define WGL_CONTEXT_PROFILE_MASK_ARB   0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001

	int attribList[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
		WGL_CONTEXT_MINOR_VERSION_ARB, 1,
		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0
	};

	HGLRC newContext = wglCreateContextAttribsARB(hdc, 0, attribList);
	if (newContext == nullptr || wglMakeCurrent(hdc, newContext) == FALSE)
		return false;
	wglDeleteContext((HGLRC)context);

	window->HasGLContext = true;
	return true;
}


#ifdef NXNA_ENABLE_DIRECT3D11
bool CreateDirect3DDevice(WindowInfo* window, int width, int height, ID3D11Device** pdevice, ID3D11DeviceContext** pcontext, ID3D11RenderTargetView** prenderTargetView, ID3D11DepthStencilView** pdepthStencilView, IDXGISwapChain** pswapChain)
{
	IDXGIFactory* factory;
	HRESULT result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result))
		return nullptr;

	IDXGIAdapter* adapter;
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result))
		return nullptr;

	DXGI_ADAPTER_DESC adapterDesc;
	if (FAILED(adapter->GetDesc(&adapterDesc)))
		return nullptr;

	IDXGIOutput* adapterOutput;
	if (FAILED(adapter->EnumOutputs(0, &adapterOutput)))
		return nullptr;

	unsigned int numModes;
	if (FAILED(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL)))
		return nullptr;

	DXGI_MODE_DESC* displayModeList = new DXGI_MODE_DESC[numModes];
	if (FAILED(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList)))
	{
		delete[] displayModeList;
		return nullptr;
	}

	int numerator = 0, denominator = 0;
	for (unsigned int i = 0; i < numModes; i++)
	{
		if (displayModeList[i].Width == (unsigned int)width)
		{
			if (displayModeList[i].Height == (unsigned int)height)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
				break;
			}
		}
	}

	delete[] displayModeList;
	adapterOutput->Release();
	adapter->Release();
	factory->Release();

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = width;
	swapChainDesc.BufferDesc.Height = height;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = window->Window;
	swapChainDesc.Windowed = true;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	D3D_FEATURE_LEVEL featureLevel[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0 };

	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;
	IDXGISwapChain* swapChain;

	result = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_DEBUG, featureLevel, 3,
		D3D11_SDK_VERSION, &swapChainDesc, &swapChain, &device, nullptr, &deviceContext);

	if (FAILED(result))
		return false;

	ID3D11Texture2D* backBufferPtr;
	if (FAILED(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr)))
		return false;

	ID3D11RenderTargetView* renderTargetView;
	if (FAILED(device->CreateRenderTargetView(backBufferPtr, NULL, &renderTargetView)))
		return false;

	backBufferPtr->Release();

	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
	depthBufferDesc.Width = width;
	depthBufferDesc.Height = height;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	ID3D11Texture2D *depthStencilBuffer;
	if (FAILED(device->CreateTexture2D(&depthBufferDesc, nullptr, &depthStencilBuffer)))
		return false;

	ID3D11DepthStencilView* depthStencilView;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	if (FAILED(device->CreateDepthStencilView(depthStencilBuffer, &depthStencilViewDesc, &depthStencilView)))
		return false;

	deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
	*pdepthStencilView = depthStencilView;

#if 0
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// Set up the description of the stencil state.
	depthStencilDesc.DepthEnable = false;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = false;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	ID3D11DepthStencilState* dstate;
	if (FAILED(device->CreateDepthStencilState(&depthStencilDesc, &dstate)))
		return false;

	deviceContext->OMSetDepthStencilState(dstate, 1);
#endif

	*pdevice = device;
	*pcontext = deviceContext;
	*prenderTargetView = renderTargetView;
	*pswapChain = swapChain;
	return true;
}
#endif

bool g_quitReceived = false;

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
		// Check if the window is being closed.
	case WM_CLOSE:
	{
		g_quitReceived = true;

		return 0;
	}
	case WM_MOUSEMOVE:
	{
#ifdef NXNA_ENABLE_INPUT
		Nxna::Input::Mouse::InjectMouseMove((int)LOWORD(lparam), (int)HIWORD(lparam));
#endif

		return 0;
	}
	case WM_LBUTTONDOWN:
	{
#ifdef NXNA_ENABLE_INPUT
		Nxna::Input::Mouse::InjectMouseButton(0, true);
#endif

		return 0;
	}
	case WM_LBUTTONUP:
	{
#ifdef NXNA_ENABLE_INPUT
		Nxna::Input::Mouse::InjectMouseButton(0, false);
#endif

		return 0;
	}
	case WM_KEYDOWN:
	{
		// TODO

		return 0;
	}
	case WM_KEYUP:
	{
		// TODO

		return 0;
	}
	}

	return DefWindowProc(hwnd, umessage, wparam, lparam);
}

void HandleMessages(WindowInfo window)
{
	// Handle the windows messages.
	MSG msg;
	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void Present(WindowInfo window)
{
	if (window.HasGLContext)
	{
		HDC dc = GetDC(window.Window);
		SwapBuffers(dc);
	}
}
