#pragma once

#include <string>

#include "ContextBase.h"


#if defined(WIN32) || defined(_WIN32)
	typedef HWND DisplayHandle;
#elif defined(__linux__)
	#ifdef USE_OPENGL_DESKTOP
		typedef Display * DisplayHandle;
	#else
		typedef EGLDisplay DisplayHandle;
	#endif
#endif


//OpenGL window base class
class WindowBase
{
public:
	struct Format {
		int redSize;
		int greenSize;
		int blueSize;
		int alphaSize;
		int depthSize;
		int stencilSize;
		int samplesPerPixel;
		bool doubleBuffering;
	};

protected:
    int w; //!<Width of window.
    int h; //!<Height of window.
    bool full; //!<if the window is fullscreen
	Format format; //!<the window surface format
    std::shared_ptr<ContextBase> context;

protected:
    WindowBase(const int width, const int height, std::string title, const bool fullScreen, const Format & windowFormat);

public:
    virtual void setup();

    int getWidth() const;
    int getHeight() const;
    bool isFullscreen() const;
    
    virtual std::shared_ptr<ContextBase> & getContext();

	const Format & getFormat() const;
	static const Format getDefaultFormat();
	virtual DisplayHandle getDisplayHandle() const;
    
	virtual void setSwapInterval(int interval) const;
    virtual void swap() const;

    virtual void destroy();

    virtual ~WindowBase();
};
