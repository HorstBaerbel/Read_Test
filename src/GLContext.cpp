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
	//try to create an OpenGL <= 2.1 context first
	GLXContext gl2Context = glXCreateNewContext(display, fbConfig, GLX_RGBA_TYPE, nullptr, GL_TRUE);
	if (gl2Context == nullptr) {
		std::cout << "Failed to create a render context!" << std::endl;
		return;
	}
	//get function bindings now. we need this for glXCreateContextAttribs
	if (!getBindings()) {
		std::cout << "Failed to get all function bindings!" << std::endl;
	}
	//check if glXCreateContextAttribs is available
	if (glXCreateContextAttribs != nullptr) {
		//create a GL >= 3.0 context
		int attribs[] = { GLX_CONTEXT_MAJOR_VERSION_ARB, 3, GLX_CONTEXT_MINOR_VERSION_ARB, 0, 0};
		GLXContext gl3Context = glXCreateContextAttribs(display, fbConfig, 0, true, &attribs[0]);
		//if that worked, destroy the 2.1 context
		if (gl3Context != nullptr) {
			glXDestroyContext(display, gl2Context); //We can destroy the GL 2.0 context once the 3.0 one has bene created
		}
		context = gl3Context;
		//get function bindings again now, as we have a new context
		if (!getBindings()) {
			std::cout << "Failed to get all function bindings!" << std::endl;
		}
	}
	else {
		context = gl2Context;
	}
	//try to make render context current
	if (!glXMakeCurrent(display, window, context)) {
		destroy();
		std::cout << "Failed to make render context current!" << std::endl;
		return;
	}
	//get extensions
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
