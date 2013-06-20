#pragma once

#include <vector>
#include <string>
#include <memory>

#if defined(WIN32) || defined(_WIN32)
	//GLES on Windows? Not yet...
#elif defined(__linux__)
	#ifdef USE_OPENGL_DESKTOP
		#include  <X11/Xlib.h>
		#include  <X11/Xatom.h>
		#include  <X11/Xutil.h>
	#else
		#include <EGL/egl.h>
		#include <GLES2/gl2.h>
		#include <GLES2/gl2ext.h>
	#endif
#endif

#ifdef BCMHOST
	#include <bcm_host.h>
#endif

#include "ESContext.h"
#include "WindowBase.h"


class ESWindow : public WindowBase
{
#ifdef BCMHOST
	static EGL_DISPMANX_WINDOW_T nativeWindow;
#else
	Display * xDisplay; //!<the native X11 display
	Window xWindow; //!<the native x11 window
#endif
	EGLDisplay eglDisplay; //!<GLES display handle
	EGLNativeWindowType eglWindow; //!<GLES window handle. this in fact the window parameter casted
	EGLSurface eglSurface; //!<GLES drawing surface

	EGLint eglMajorVersion;
	EGLint eglMinorVersion;

public:
	ESWindow(const int width, const int height, std::string title, const bool fullScreen, const Format & windowFormat);

	virtual std::shared_ptr<ContextBase> & getContext();

	static const Format getDefaultFormat();
    virtual DisplayHandle getDisplayHandle() const;

    virtual void setSwapInterval(int interval) const;
    virtual void swap() const;

	virtual void destroy();

	virtual ~ESWindow();
};
