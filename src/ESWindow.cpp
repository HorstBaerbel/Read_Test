#include "ESWindow.h"

#include <iostream>


EGL_DISPMANX_WINDOW_T ESWindow::nativeWindow;


ESWindow::ESWindow(const int width, const int height, std::string title, const bool fullScreen, const Format & windowFormat)
	:  WindowBase(width, height, title, fullScreen, windowFormat), eglWindow(nullptr), eglSurface(nullptr)
#ifdef BCMHOST
#else
	, xDisplay(nullptr), xWindow(nullptr)
#endif
{
#ifdef BCMHOST //Raspberry Pi path
	DISPMANX_ELEMENT_HANDLE_T dispman_element;
	DISPMANX_DISPLAY_HANDLE_T dispman_display;
	DISPMANX_UPDATE_HANDLE_T dispman_update;
	VC_RECT_T dst_rect;
	VC_RECT_T src_rect;

	if (fullScreen) {
		//get display size currently connected
		uint32_t displayWidth;
		uint32_t displayHeight;
		graphics_get_display_size(0, &displayWidth, &displayHeight);
		w = displayWidth;
		h = displayHeight;
	}

	dst_rect.x = 0;
	dst_rect.y = 0;
	dst_rect.width = w;
	dst_rect.height = h;

	src_rect.x = 0;
	src_rect.y = 0;
	src_rect.width = w << 16;
	src_rect.height = h << 16;

	dispman_display = vc_dispmanx_display_open(0 /* LCD */);
	dispman_update = vc_dispmanx_update_start(0);
	dispman_element = vc_dispmanx_element_add ( dispman_update, dispman_display, 0, &dst_rect, 0, &src_rect, DISPMANX_PROTECTION_NONE, 0, 0, DISPMANX_NO_ROTATE);

	nativeWindow.element = dispman_element;
	nativeWindow.width = width;
	nativeWindow.height = height;
	vc_dispmanx_update_submit_sync(dispman_update);

	eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);

#else //X11 path

	//get native X11 display
	xDisplay = XOpenDisplay(nullptr);
	if (xDisplay == nullptr) {
		std::cout << "Failed to get an X11 display handle!" << std::endl;
		return;
	}

	//get default root window of display
	Window rootWindow = DefaultRootWindow(xDisplay);
	//create new X11 window
	XSetWindowAttributes windowAttributes;
	windowAttributes.event_mask = ExposureMask | PointerMotionMask | KeyPressMask;
	xWindow = XCreateWindow(xDisplay, rootWindow, 0, 0, w, h, 0, CopyFromParent, InputOutput, CopyFromParent, CWEventMask, &windowAttributes);
	if (display == nullptr) {
		std::cout << "Failed to create an X11 window!" << std::endl;
		return;
	}
	eglWindow = (EGLNativeWindowType)window;

	//change window attributes
	XSetWindowAttributes overrideAttributes;
	overrideAttributes.override_redirect = FALSE;
	XChangeWindowAttributes(xDisplay, xWindow, CWOverrideRedirect, &overrideAttributes);

	//change window hints
	XWMHints hints;
	hints.input = TRUE;
	hints.flags = InputHint;
	XSetWMHints(xDisplay, xWindow, &hints);

	//make the window visible on the screen
	XMapWindow (xDisplay, xWindow);
	XStoreName (xDisplay, xWindow, title);

	//get identifiers for the provided atom name strings
	XEvent xEvent;
	memset(&xEvent, 0, sizeof(xEvent));
	xEvent.type                 = ClientMessage;
	xEvent.xclient.window       = xWindow;
	xEvent.xclient.message_type = XInternAtom(xDisplay, "_NET_WM_STATE", FALSE);
	xEvent.xclient.format       = 32;
	xEvent.xclient.data.l[0]    = 1;
	xEvent.xclient.data.l[1]    = FALSE;
	XSendEvent(xDisplay, rootWindow, FALSE, SubstructureNotifyMask, &xEvent);

	eglDisplay = eglGetDisplay((EGLNativeDisplayType)xDisplay);

#endif //end of Raspberry Pi / X11 differences

	//get EGL display
	if (eglDisplay == EGL_NO_DISPLAY) {
		destroy();
		std::cout << "Failed to get an EGL display!" << std::endl;
		return;
	}

	//initialize EGL
	if (!eglInitialize(eglDisplay, &eglMajorVersion, &eglMinorVersion)) {
		destroy();
		std::cout << "Failed to initialize EGL!" <<std::endl;
		return;
	}
	else {
		std::cout << "EGL version is " << eglMajorVersion << "." << eglMinorVersion << std::endl;
	}

	//get configs
//	EGLint numConfigs;
//	if (!eglGetConfigs(eglDisplay, nullptr, 0, &numConfigs)) {
//		destroy();
//		std::cout << "Failed to get EGL configs!" <<std::endl;
//		return;
//	}

	//setup surface attributes
	EGLint attribList[] = {
		EGL_RED_SIZE,        format.redSize,
		EGL_GREEN_SIZE,      format.greenSize,
		EGL_BLUE_SIZE,       format.blueSize,
		EGL_ALPHA_SIZE,      format.alphaSize,
		EGL_DEPTH_SIZE,      format.depthSize,
		EGL_STENCIL_SIZE,    format.stencilSize,
		EGL_SAMPLE_BUFFERS,  ((format.samplesPerPixel > 1) ? 1 : 0),
		EGL_SAMPLES,         format.samplesPerPixel,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_NONE,            EGL_NONE
	};
	//choose config
	EGLConfig config;
	EGLint numConfigs = 0;
	if (!eglChooseConfig(eglDisplay, attribList, &config, 1, &numConfigs)) {
		destroy();
		std::cout << "Failed to choose EGL config with RGBA" << attribList[1] << attribList[3] << attribList[5] << attribList[7];
		std::cout << ", depth " << attribList[9] << ", stencil " << attribList[11];
		if (attribList[13]) {
			std::cout << ", " << attribList[15] << "x multisampling!" << std::endl;
		}
		else {
			std::cout << ", no multisampling!" << std::endl;
		}
		return;
	}
	else {
		std::cout << "Chose EGL config with RGBA" << attribList[1] << attribList[3] << attribList[5] << attribList[7];
		std::cout << ", depth " << attribList[9] << ", stencil " << attribList[11];
		if (attribList[13]) {
			std::cout << ", " << attribList[15] << "x multisampling!" << std::endl;
		}
		else {
			std::cout << ", no multisampling!" << std::endl;
		}
	}

	//create an EGL surface
	//NOTE: We COULD actually pass the attributes EGL_RENDER_BUFFER, EGL_SINGLE_BUFFER here to get single buffering
	//but it isn't supported on some systems. We ignore it atm...
	eglSurface = eglCreateWindowSurface(eglDisplay, config, eglWindow, nullptr);
	if (eglSurface == EGL_NO_SURFACE) {
		destroy();
		std::cout << "Failed to get an EGL surface!" <<std::endl;
		return;
	}

	//create a EGL context
	context = std::shared_ptr<ESContext>(new ESContext(eglDisplay, eglSurface, config));
	if (!context->isValid()) {
		destroy();
		std::cout << "Failed to create an EGL context!" <<std::endl;
        	return;
	}

	std::cout << "Sucessfully created " << w << "x" << h << " window" << std::endl;

	//set viewport to whole window
	glViewport(0, 0, w, h);
}

std::shared_ptr<ContextBase> & ESWindow::getContext()
{
    return context;
}

const WindowBase::Format ESWindow::getDefaultFormat()
{
#ifdef BCMHOST
	//NOTE: The RGBA5650 surface format without depth and stencil seems to be the fastest when blitting framebuffers!
	Format f = {5, 6, 5, 0, 0, 0, 1, true};
	return f;
#else
	return WindowBase::getDefaultFormat();
#endif
}

DisplayHandle ESWindow::getDisplayHandle() const
{
	return eglDisplay;
}

void ESWindow::setSwapInterval(int interval) const
{
    eglSwapInterval(eglDisplay, interval);
}

void ESWindow::swap() const
{
    eglSwapBuffers(eglDisplay, eglSurface);
}

void ESWindow::destroy()
{
	//free context
	context->destroy();
	//free up surface and display
	if (eglSurface != nullptr) {
		eglDestroySurface(eglDisplay, eglSurface);
		eglSurface = nullptr;
	}
	if (eglDisplay != nullptr) {
		eglTerminate(eglDisplay);
		eglDisplay = nullptr;
	}
}

ESWindow::~ESWindow()
{
	destroy();
}
