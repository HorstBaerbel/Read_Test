#pragma once

#if defined(WIN32) || defined(_WIN32)
	#include <Windows.h>
#elif defined(__linux__)
	#include <X11/Xlib.h>
	#include <X11/Xatom.h>
	#include <X11/Xutil.h>
	#include <X11/keysym.h>   
	#include <X11/extensions/xf86vmode.h>
#endif

#include "GLContext.h"
#include "WindowBase.h"


class GLWindow : public WindowBase
{
#if defined(WIN32) || defined(_WIN32)
    HWND hWND; //!<Window handle of parent window
    HDC hDC;   //!<DC of window (if rendering to a window)
	DEVMODE oldDisplayMode; //!<The old windows display mode before the mode switch
#elif defined(__linux__)
	Display * xDisplay; //!<the X11 display we're rendering to
	Window xWindow; //!<the X11 window we're rendering to
	int xScreenId; //!<the X11 screen that is being used
	XF86VidModeModeInfo oldXMode; //!<the old X11 video mode before switch to fullscreen
#endif

public:
#if defined(WIN32) || defined(_WIN32)
    GLWindow(const int width, const int height, std::string title, const bool fullScreen, const Format & windowFormat, WNDPROC wndProc);
#elif defined(__linux__)
	GLWindow(const int width, const int height, std::string title, const bool fullScreen, const Format & windowFormat);
#endif
	virtual void setup();

	static const Format getDefaultFormat();
	virtual DisplayHandle getDisplayHandle() const;

    virtual void setSwapInterval(int interval) const;
    virtual void swap() const;

    virtual void destroy();

    virtual ~GLWindow();
};
