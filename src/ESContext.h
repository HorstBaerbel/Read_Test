#pragma once

#if defined(WIN32) || defined(_WIN32)
	//?
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

#include "ContextBase.h"


class ESContext : public ContextBase
{
	EGLDisplay eglDisplay; //!<display
	EGLContext eglContext; //!<device context
	EGLSurface eglSurface; //!<rendering surface

public:
	ESContext(EGLDisplay & display, EGLSurface & surface, EGLConfig & config);

	virtual bool makeCurrent();
	virtual void destroy();

	virtual bool isDirect() const;

	virtual bool isValid() const;

    virtual ~ESContext();
};
