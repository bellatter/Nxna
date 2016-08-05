#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/glx.h>
#include <cstdio>

#include "Common.h"

bool g_quitReceived = false;

typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);

static bool g_ctxErrorOccurred = false;
static Atom g_wmDeleteMessage;
static int ctxErrorHandler( Display *dpy, XErrorEvent *ev )
{
    g_ctxErrorOccurred = true;
    return 0;
}

bool CreateGameWindow(int width, int height, bool fullscreen, const char* title, WindowInfo* result)
{
	Display* display = XOpenDisplay(NULL);

	if (!display)
	{
		return false;
	}

	// Get a matching FB config
	static int visual_attribs[] =
	{
	GLX_X_RENDERABLE    , True,
	GLX_DRAWABLE_TYPE   , GLX_WINDOW_BIT,
	GLX_RENDER_TYPE     , GLX_RGBA_BIT,
	GLX_X_VISUAL_TYPE   , GLX_TRUE_COLOR,
	GLX_RED_SIZE        , 8,
	GLX_GREEN_SIZE      , 8,
	GLX_BLUE_SIZE       , 8,
	GLX_ALPHA_SIZE      , 8,
	GLX_DEPTH_SIZE      , 24,
	GLX_STENCIL_SIZE    , 8,
	GLX_DOUBLEBUFFER    , True,
	//GLX_SAMPLE_BUFFERS  , 1,
	//GLX_SAMPLES         , 4,
	None
	};

	int glx_major, glx_minor;
 
	// FBConfigs were added in GLX version 1.3.
	if ( !glXQueryVersion( display, &glx_major, &glx_minor) || 
		((glx_major == 1) && (glx_minor < 3)) || (glx_major < 1))
	{
		return false;
	}

	int fbcount;
	auto screen = DefaultScreen(display);
	GLXFBConfig* fbc = glXChooseFBConfig(display, screen, visual_attribs, &fbcount);
	if (!fbc)
	{
		return false;
	}

	// Pick the FB config/visual with the most samples per pixel
  	int best_fbc = -1, worst_fbc = -1, best_num_samp = -1, worst_num_samp = 999;

	int i;
	for (i=0; i<fbcount; ++i)
	{
		XVisualInfo *vi = glXGetVisualFromFBConfig( display, fbc[i] );
		if ( vi )
		{
		int samp_buf, samples;
		glXGetFBConfigAttrib( display, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf );
		glXGetFBConfigAttrib( display, fbc[i], GLX_SAMPLES       , &samples  );
      
		if ( best_fbc < 0 || samp_buf && samples > best_num_samp )
			best_fbc = i, best_num_samp = samples;
		if ( worst_fbc < 0 || !samp_buf || samples < worst_num_samp )
			 worst_fbc = i, worst_num_samp = samples;
		}
		XFree( vi );
	}

	GLXFBConfig bestFbc = fbc[ best_fbc ];

	// Be sure to free the FBConfig list allocated by glXChooseFBConfig()
	XFree( fbc );

	// Get a visual
	XVisualInfo *vi = glXGetVisualFromFBConfig( display, bestFbc );

	XSetWindowAttributes swa;
	Colormap cmap;
	swa.colormap = cmap = XCreateColormap( display, RootWindow(display, vi->screen), vi->visual, AllocNone);
	swa.background_pixmap = None ;
	swa.border_pixel      = 0;
	swa.event_mask        = StructureNotifyMask;

	Window win = XCreateWindow( display, RootWindow( display, vi->screen ), 
                              0, 0, width, height, 0, vi->depth, InputOutput, 
                              vi->visual, 
                              CWBorderPixel|CWColormap|CWEventMask, &swa );
	if (!win)
	{
		return false;
	}

	// Done with the visual info data
	XFree( vi );

	// register interest in the delete window message
	g_wmDeleteMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
	XSetWMProtocols(display, win, &g_wmDeleteMessage, 1);

	XStoreName( display, win, title);

	XMapWindow( display, win );

	// NOTE: It is not necessary to create or make current to a context before
	// calling glXGetProcAddressARB
	glXCreateContextAttribsARBProc glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)glXGetProcAddressARB((const GLubyte *)"glXCreateContextAttribsARB");

	if (!glXCreateContextAttribsARB )
	{
		return false;
	}

	GLXContext ctx = 0;

	// Install an X error handler so the application won't exit if GL 3.0
	// context allocation fails.
	//
	// Note this error handler is global.  All display connections in all threads
	// of a process use the same error handler, so be sure to guard against other
	// threads issuing X commands while this code is running.
	g_ctxErrorOccurred = false;
	int (*oldHandler)(Display*, XErrorEvent*) = XSetErrorHandler(&ctxErrorHandler);

	int context_attribs[] =
	{
		GLX_CONTEXT_MAJOR_VERSION_ARB, 4,
		GLX_CONTEXT_MINOR_VERSION_ARB, 1,
		GLX_CONTEXT_FLAGS_ARB        , GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
		None
	};

	ctx = glXCreateContextAttribsARB(display, bestFbc, 0, True, context_attribs );

	// Sync to ensure any errors generated are processed.
	XSync(display, False );
	if (g_ctxErrorOccurred || !ctx )
	      return false;

	// Restore the original error handler
	XSetErrorHandler( oldHandler );

	// Verifying that context is a direct context
	if (!glXIsDirect(display, ctx))
		return false;

	glXMakeCurrent(display, win, ctx );

	result->Display = display;
	result->Window = win;
	result->Context = ctx;

	return true;
}

void ShowGameWindow(WindowInfo window)
{
	// nothing
}

void DestroyGameWindow(WindowInfo window)
{
	glXMakeCurrent((Display*)window.Display, None, NULL);
	glXDestroyContext((Display*)window.Display, (GLXContext)window.Context);
	XDestroyWindow((Display*)window.Display, (Window)window.Window);
	XCloseDisplay((Display*)window.Display);
}

bool CreateOpenGLContext(WindowInfo* window)
{
	return true;
}

void HandleMessages(WindowInfo window)
{
	while(XEventsQueued((Display*)window.Display, QueuedAfterFlush) > 0)
	{
		XEvent e;
		XNextEvent((Display*)window.Display, &e);

		if (e.type == ClientMessage)
		{
			if (e.xclient.data.l[0] == g_wmDeleteMessage)
			{
				g_quitReceived = true;
			}
		}
	}
}

void Present(WindowInfo window)
{
	glXSwapBuffers((Display*)window.Display, (Window)window.Window);
}

