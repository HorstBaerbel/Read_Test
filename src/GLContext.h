#pragma once

#if defined(WIN32) || defined(_WIN32)
	#include <Windows.h>
#elif defined(__linux__)
	#include<X11/X.h>
	#include<X11/Xlib.h>
	#include<GL/glx.h>
#endif

#include <GL/gl.h>

#include "ContextBase.h"


class GLContext : public ContextBase
{
#if defined(WIN32) || defined(_WIN32)
    HGLRC hRC; //!<OpenGL rendering-context for surface
    HDC hDC;   //!<DC of window (if rendering to a window)
#elif defined(__linux__)
	Display * xDisplay; //!<the X11 display we're rendering to
	Window xWindow; //!<the X11 window we're rendering to
	GLXContext context; //!<The GLX context used for rendering
	int glxVersionMajor;
	int glxVersionMinor;
#endif

public:
#if defined(WIN32) || defined(_WIN32)
	GLContext(HDC dc);
#elif defined(__linux__)
	GLContext(Display * display, Window & window, GLXFBConfig & fbConfig);
#endif

	virtual bool makeCurrent();
	virtual void destroy();

	virtual bool isDirect() const;

	virtual bool isValid() const;

	virtual ~GLContext();
};
