#include "GLContext.h"

#include <iostream>


#if defined(WIN32) || defined(_WIN32)
GLContext::GLContext(HDC dc)
	: hDC(nullptr), hRC(nullptr)
{
	//create OpenGL context
	HGLRC rc = wglCreateContext(dc);
	if (rc == nullptr) {
		destroy();
		std::cout << "Failed to create a render context!" << std::endl;
		return;
	}
	//make context current
	if(!wglMakeCurrent(dc, rc)) {
		destroy();
        std::cout << "Failed to make render context current!" << std::endl;
		return;
	}
	//get extensions
	getExtensions();
	//get function bindings here
	if (!getBindings()) {
        std::cout << "Failed to get all function bindings!" << std::endl;
	}
	//setup members
	hDC = dc;
	hRC = rc;
}

#elif defined(__linux__)

typedef GLXContext (GLAPIENTRYP glXCreateContextAttribsProc)(Display *dpy, GLXFBConfig config, GLXContext share_context, Bool direct, const int *attrib_list);

static bool ctxErrorOccurred = false;
static int contextErrorHandler(Display * /*display*/, XErrorEvent * event)
{
    ctxErrorOccurred = true;
    return 0;
}

GLContext::GLContext(Display * display, Window & window, GLXFBConfig & fbConfig)
	: xDisplay(nullptr), xWindow(0), context(nullptr), glxVersionMajor(1), glxVersionMinor(0)
{
	//framebuffer configs were added in GLX version 1.3 so check for that
	if (!glXQueryVersion(display, &glxVersionMajor, &glxVersionMinor))	{
		std::cout << "Failed to get the GLX version!" << std::endl;
		return;
	}
	if (glxVersionMajor < 1 || (glxVersionMajor == 1 && glxVersionMinor < 3)) {
		std::cout << "GLX version 1.3 or higher needed!" << std::endl;
		return;
	}
    //try to bind glXGetProcAddress or glXGetProcAddressARB
    //not that is not necessary to create or make current to a context before...
    glXCreateContextAttribsProc glXCreateContextAttribs = nullptr;
    glXCreateContextAttribs = (glXCreateContextAttribsProc)glXGetProcAddress((const GLubyte *) "glXCreateContextAttribs");
    if (glXCreateContextAttribs == nullptr) {
        glXCreateContextAttribs = (glXCreateContextAttribsProc)glXGetProcAddress((const GLubyte *) "glXCreateContextAttribsARB");
    }
    // Set global error handler. All display connections in all threads of a process use the same error handler, 
    // so be sure to guard against other threads issuing X commands while this code is running.
    int (*oldHandler)(Display*, XErrorEvent*) = XSetErrorHandler(&contextErrorHandler);
	//check if glXCreateContextAttribs is available
	if (glXCreateContextAttribs != nullptr) {
        //create a GL >= 3.0 context
        std::cout << "glXCreateContextAttribs is available.\nTrying to get an OpenGL 3.0 context... ";
		int attribs[] = { GLX_CONTEXT_MAJOR_VERSION_ARB, 3, GLX_CONTEXT_MINOR_VERSION_ARB, 0, 0};
		context = glXCreateContextAttribs(display, fbConfig, 0, true, &attribs[0]);
        if (context == nullptr || ctxErrorOccurred) {
            std::cout << "failed!" << std::endl;
            ctxErrorOccurred = false;
        }
        else {
            std::cout << "worked." << std::endl;
        }
	}
	if (context == nullptr || ctxErrorOccurred) {
        //try to create an OpenGL <= 2.1 context
        std::cout << "Trying to get an OpenGL 2.1 context... ";
	    context = glXCreateNewContext(display, fbConfig, GLX_RGBA_TYPE, nullptr, GL_TRUE);
        if (context == nullptr || ctxErrorOccurred) {
            std::cout << "failed!" << std::endl;
            ctxErrorOccurred = false;
        }
        else {
            std::cout << "worked." << std::endl;
        }
	}
    // sync to ensure any errors generated are processed.
    XSync(display, false);
    //reset error handler
    XSetErrorHandler(oldHandler);
    //check if we successfully created a context
    if (context == nullptr) {
	    std::cout << "Failed to create a render context!" << std::endl;
	    return;
    }
    // check if we got a direct context
    if (glXIsDirect(display, context)) {
        std::cout << "Got a direct context." << std::endl;
    }
    else {
        std::cout << "Got an indirect context." << std::endl;
    }
	//get function bindings now
	if (!getBindings()) {
		std::cout << "Failed to get all function bindings!" << std::endl;
	}
	//try to make render context current
	if (!glXMakeCurrent(display, window, context)) {
		destroy();
		std::cout << "Failed to make render context current!" << std::endl;
		return;
	}
    //get info about and extensions
    getImplementationInfo();
	getExtensions();
	//setup members
	xDisplay = display;
	xWindow = window;
}
#endif

bool GLContext::makeCurrent()
{
#if defined(WIN32) || defined(_WIN32)
	if(!wglMakeCurrent(hDC, hRC)) {
#elif defined(__linux__)
	if (!glXMakeCurrent(xDisplay, xWindow, context)) {
#endif
		std::cout << "Failed to make render context current!" << std::endl;
		return false;
	}
	return true;
}

void GLContext::destroy()
{
#if defined(WIN32) || defined(_WIN32)
	if (hRC != nullptr) {
		//Try to release the rendering context. HDC is ignored in this case
		wglMakeCurrent(nullptr, nullptr);
		//Try to delete the rendering context
		wglDeleteContext(hRC);
		hRC = nullptr;
	}
#elif defined(__linux__)
	if (context != nullptr) {
		//make no context current
		glXMakeCurrent(xDisplay, None, nullptr);
		//destroy OpenGL context
		glXDestroyContext(xDisplay, context);
		context = nullptr;
	}
#endif
}

bool GLContext::isDirect() const
{
#if defined(WIN32) || defined(_WIN32)
	return true;
#elif defined(__linux__)
	return glXIsDirect(xDisplay, context);
#endif
}

bool GLContext::isValid() const
{
#if defined(WIN32) || defined(_WIN32)
	return (hDC != nullptr && hRC != nullptr);
#elif defined(__linux__)
	return (context != nullptr);
#endif
}

GLContext::~GLContext()
{
	destroy();
}
