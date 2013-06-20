#include "ESContext.h"

#include <iostream>


ESContext::ESContext(EGLDisplay & display, EGLSurface & surface, EGLConfig & config)
	: eglDisplay(nullptr), eglContext(nullptr), eglSurface(nullptr)
{
	EGLint attribList[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE, EGL_NONE
	};

	//create context
	eglContext = eglCreateContext(display, config, EGL_NO_CONTEXT, attribList);
	if (eglContext == EGL_NO_CONTEXT) {
		destroy();
		std::cout << "Failed to get an EGL context!" <<std::endl;
		return;
	}
	//make context current
	if(!eglMakeCurrent(display, surface, surface, eglContext)) {
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
	eglDisplay = display;
	eglSurface = surface;
}

bool ESContext::makeCurrent()
{
	if(!eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext)) {
		std::cout << "Failed to make render context current!" << std::endl;
		return false;
	}
	return true;
}

void ESContext::destroy()
{
	if (eglContext != nullptr) {
		//make context not current
		eglMakeCurrent(eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		//destroy context
		eglDestroyContext(eglDisplay, eglContext);
		eglDisplay = nullptr;
		eglSurface = nullptr;
		eglContext = nullptr;
	}
}

bool ESContext::isDirect() const
{
	return true;
}

bool ESContext::isValid() const
{
	return (eglContext != nullptr);
}

ESContext::~ESContext()
{
    destroy();
}
