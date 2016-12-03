#include "GLWindow.h"

#include <iostream>
#include <GL/gl.h>

#if defined(WIN32) || defined(_WIN32)
	#include <GL/wglext.h>
#endif


//win32-specific functions -------------------------------------------------------------------------------------

#if defined(WIN32) || defined(_WIN32)
GLWindow::GLWindow(const int width, const int height, std::string title, const bool fullScreen, const Format & windowFormat, WNDPROC wndProc)
    : WindowBase(width, height, title, fullScreen, windowFormat), hWND(nullptr), hDC(nullptr)
{
	HINSTANCE instance = GetModuleHandle(nullptr);
	if (instance == NULL) {
		//no need to continue if we can't create a GL window
		return;
	}

	WNDCLASS windowClass;
	windowClass.hIcon         = NULL; //LoadIcon(NULL, IDI_INFORMATION);
	windowClass.lpszMenuName  = NULL;
	windowClass.style		  = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraws The Window For Any Movement / Resizing
    windowClass.cbClsExtra    = 0;
    windowClass.cbWndExtra    = 0;
	windowClass.lpfnWndProc	  = (WNDPROC)wndProc;			// WindowProc Handles Messages
	windowClass.hInstance	  = instance;				// Set The Instance
	//If you want to have an icon, include one in the resources, the program will show the first one it can find
	windowClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);			// Class Background Brush Color
	windowClass.hCursor		  = NULL; //LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	windowClass.lpszClassName = title.c_str();				// Sets The Applications Classname
	if (RegisterClass(&windowClass) == 0) {
        std::cout << "Registering window class failed!" << std::endl;
		return;
	}

	DWORD windowStyle = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE; //define our window style. intially visible
	//DWORD windowExtendedStyle = WS_EX_APPWINDOW;						// Define The Window's Extended Style

	RECT windowRect = {0, 0, w, h};	// Define Our Window Coordinates

	if (fullScreen)	{
		//get old mode
		ZeroMemory (&oldDisplayMode, sizeof (DEVMODE)); //make sure memory is cleared
		EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &oldDisplayMode);
		//try to set new mode
		DEVMODE dmScreenSettings;											// Device Mode
		ZeroMemory (&dmScreenSettings, sizeof (DEVMODE));					// Make Sure Memory Is Cleared
		dmScreenSettings.dmSize				= sizeof (DEVMODE);				// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth		= w;						// Select Screen Width
		dmScreenSettings.dmPelsHeight		= h;						// Select Screen Height
		dmScreenSettings.dmBitsPerPel		= 32;					// Select Bits Per Pixel
		dmScreenSettings.dmFields			= DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
		if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) {
			//fullscreen mode switch failed. run in windowed mode instead
            std::cout << "Mode switch failed. Running in windowed mode instead." << std::endl;
			full = false;
		}
		else {
			//switching to fullscreen worked. create top window covering everything else
			windowStyle |= WS_POPUP; //popup window
			//windowExtendedStyle |= WS_EX_TOPMOST; //top-most window
		}
	}
	else {																// If Fullscreen Was Not Selected
		//set the WindowStyle to a non-maximizable, non-resizeable window
		windowStyle |= WS_OVERLAPPEDWINDOW ^ WS_MAXIMIZEBOX ^ WS_SIZEBOX;
		// Adjust Window, Account For Window Borders
		//AdjustWindowRectEx(&windowRect, windowStyle, 0, windowExtendedStyle);
		AdjustWindowRect(&windowRect, windowStyle, false);
	}

    int multisamplingSupported = format.samplesPerPixel > 1 ? -1 : 0; //-1 = check for multisampling, 0 = not supported, 1 = supported, enable
    //goto here to re-create the window again after checking for multisampling
createWindowAgain:

	// Create The OpenGL Window
	hWND = CreateWindow(title.c_str(),	                    // Class Name
						title.c_str(),					    // Window Title
						windowStyle,						// Window Style
						CW_USEDEFAULT, CW_USEDEFAULT,		// Window X,Y Position
						windowRect.right - windowRect.left,	// Window Width
						windowRect.bottom - windowRect.top,	// Window Height
						HWND_DESKTOP,						// Desktop Is Window's Parent
						0,									// No Menu
						instance,                           // Pass The program Instance
						NULL);

	if (hWND == nullptr) {
		destroy();
        std::cout << "Failed to create a window!" <<std::endl;
		return;
	}

	//get device context
    hDC = GetDC(hWND);
	if (hDC == nullptr) {
		destroy();
        std::cout << "Failed to get a device context!" << std::endl;
		return;
	}

    static int pfID = 0; //pixel format id.
	//Choose pixel format
	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),           //size of structure
		1,                                       //version number
		PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | (full ? 0 : PFD_DRAW_TO_WINDOW), //opengl-flags, support drawing to window
		PFD_TYPE_RGBA,                           //pixel-type is RGBA
		format.redSize + format.greenSize + format.blueSize + format.alphaSize, //bits in color-buffer
		0,0,0,                                   //RGB-shift-bits
		0,0,0,                                   //RGB-shift-sizes
		format.alphaSize,										 //alpha-buffer info
		0,                                       //no alpha-shift-bit
		0,0,0,0,                                 //accumulation buffer
		format.depthSize,                        //z-buffer depth
		format.stencilSize,                      //stencil-buffer depth
		0,                                       //no auxilliary buffers
		0,                                       //Layer type. not used anymore
		0,                                       //reserved (always=0)
		0,                                       //no layer mask
		0,                                       //no visible mask
		0                                        //no damage mask
	};

    if (!fullScreen) {
        pfd.dwFlags |= PFD_DRAW_TO_WINDOW;
    }

    if (multisamplingSupported < 1) {
        pfID = ChoosePixelFormat(hDC, &pfd);
    }

	if (pfID == 0) {
		destroy();
        std::cout << "Failed to choose pixel format!" << std::endl;
		return;
	}

	if (!SetPixelFormat(hDC, pfID, &pfd)) {
		destroy();
        std::cout << "Failed to set pixel format!" << std::endl;
		return;
	}

	//Create rendering context for it and activate it
	context = std::shared_ptr<GLContext>(new GLContext(hDC));
	if (!context->isValid()) {
		destroy();
        std::cout << "Failed to create a render context!" << std::endl;
		return;
	}

    //now get multisampling information
    //check if we need to check for multisampling support
    if (multisamplingSupported < 0) {
        //choose new multisampling pixel format
	    int iAttributes[] = {
            WGL_DRAW_TO_WINDOW_ARB, (full ? GL_FALSE : GL_TRUE),
		    WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
		    WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
            WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
		    WGL_COLOR_BITS_ARB, format.redSize + format.greenSize + format.blueSize,
		    WGL_ALPHA_BITS_ARB, format.alphaSize,
		    WGL_DEPTH_BITS_ARB, format.depthSize,
		    WGL_STENCIL_BITS_ARB, format.stencilSize,
		    WGL_DOUBLE_BUFFER_ARB, (format.doubleBuffering ? GL_TRUE : GL_FALSE),
		    0, 0,
		    0, 0, 
			0, 0
        };
        if (format.samplesPerPixel > 1) {
            iAttributes[18] = WGL_SAMPLE_BUFFERS_ARB;
            iAttributes[19] = GL_TRUE;
		    iAttributes[20] = WGL_SAMPLES_ARB;
            iAttributes[21] = format.samplesPerPixel;
        }

        //try to select a multisampling pixel format
	    multisamplingSupported = context->wglChoosePixelFormat(hDC, (const int *)&iAttributes, nullptr, 1, &pfID, (UINT *)&iAttributes[23]);
        if (multisamplingSupported && iAttributes[23] > 0) {
            //destroy window again. we'll create a new one
            destroy();
            //now do the window creation procedure again
            goto createWindowAgain;
        }
		//else no multisampling
		format.samplesPerPixel = 1;
    }

    if (fullScreen) {
	    //hide cursor
	    ShowCursor(false);
    }
	//set up some standard OpenGL stuff
	setup();
}

#elif defined(__linux__)

GLWindow::GLWindow(const int width, const int height, std::string title, const bool fullScreen, const Format & windowFormat)
	: WindowBase(width, height, title, fullScreen, windowFormat), xDisplay(nullptr), xWindow(0), xScreenId(0)
{
	//get native X11 display
	xDisplay = XOpenDisplay(nullptr);
	if (xDisplay == nullptr) {
		std::cout << "Failed to get an X11 display handle!" << std::endl;
		return;
	}
	//get the default screen id
	xScreenId = DefaultScreen(xDisplay);

	//check if GLX is supported
	if (!glXQueryExtension(xDisplay, nullptr, nullptr)) {
		std::cout << "GLX is not supported!" << std::endl;
		return;
	}
	else {
		std::cout << "GLX is supported." << std::endl;
	}

	//framebuffer configs were added in GLX version 1.3 so check for that
	int major, minor;
	if (!glXQueryVersion(xDisplay, &major, &minor))	{
		std::cout << "Failed to get the GLX version!" << std::endl;
		return;
	}
	if (major < 1 || (major == 1 && minor < 3)) {
		std::cout << "GLX version 1.3 or higher needed!" << std::endl;
		return;
	}

	//check for XF86 video mode extension
	int vmMajor, vmMinor;
	if (!XF86VidModeQueryVersion(xDisplay, &vmMajor, &vmMinor)) {
		std::cout << "XF86 VideoMode extension version not available!" << std::endl;
		return;
	}
	else {
		std::cout << "XF86 VideoMode extension version is " << vmMajor << "." << vmMinor << std::endl;
	}

	//get all X11 display modes for the default screen
	XF86VidModeModeInfo ** modes = nullptr;
	int nrOfModes = 0;
	if (!XF86VidModeGetAllModeLines(xDisplay, xScreenId, &nrOfModes, &modes)) {
		destroy();
		std::cerr << "Could not query the video modes!" << std::endl;
		return;
	}
	else {
		std::cerr << nrOfModes << " video modes found." << std::endl;
	}
	//store the current diplay mode
	oldXMode = *modes[0];
	XF86VidModeModeInfo bestMode = oldXMode;
	int bestModeIndex = -1;
	//if we want fullscreen find a matching fullscreen mode
	if (fullScreen) {
findModesAgain:
		//find best fitting mode
		for (int i = 0; i < nrOfModes; i++) {
			if (modes[i] != nullptr && (modes[i]->hdisplay == w) &&	(modes[i]->vdisplay == w)) {
				bestModeIndex = i;
				bestMode = *modes[bestModeIndex];
				break;
			}
		}
		//found a mode?
		if (bestModeIndex == -1) {
			if (w != 640 || h != 480) {
				//no. use default 640x480 and try again
				std::cerr << "Could not find a suitable graphics mode for " << width << "x" << height << ". Trying 640x480." << std::endl;
				w = 640;
				h = 480;
				goto findModesAgain;
			}
			else {
				//no. run windowed
				std::cerr << "Could not find a suitable graphics mode. Running windowed!" << std::endl;
				full = false;
			}
		}
	}
	//free video modes
	XFree(modes);
	//build framebuffer config
	int iAttributes[] = {
		GLX_X_RENDERABLE, True,
		GLX_RENDER_TYPE, GLX_RGBA_BIT,
		GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
		GLX_RED_SIZE, format.redSize,
		GLX_GREEN_SIZE, format.greenSize,
		GLX_BLUE_SIZE, format.blueSize,
		GLX_ALPHA_SIZE, format.alphaSize,
		GLX_DEPTH_SIZE, format.depthSize,
		GLX_STENCIL_SIZE, format.stencilSize,
		GLX_DOUBLEBUFFER, (format.doubleBuffering ? GL_TRUE : GL_FALSE),
        GLX_CONTEXT_MAJOR_VERSION_ARB, 2,
        GLX_CONTEXT_MINOR_VERSION_ARB, 1,
		None, None,
		None, None,
		None, None,
		None, None
	}; //this crashes if GLX_SAMPLES_ARB is set, but 1. thus we do it differently
	if (!full) {
		iAttributes[24] = GLX_DRAWABLE_TYPE;
		iAttributes[25] = GLX_WINDOW_BIT;
	}
        if (format.samplesPerPixel > 1) {
		iAttributes[26] = GLX_SAMPLE_BUFFERS_ARB;
		iAttributes[27] = GL_TRUE;
		iAttributes[28] = GLX_SAMPLES;
		iAttributes[29] = format.samplesPerPixel;
	}
	//get a matching framebuffer config
	int fbCount = 0;
	GLXFBConfig * fbConfig = glXChooseFBConfig(xDisplay, xScreenId, iAttributes, &fbCount);
	if (fbConfig == nullptr) {
		destroy();
		std::cout << "Failed to get a suitable framebuffer configuration!" << std::endl;
		return;
	}
	else {
		std::cout << fbCount << " suitable framebuffer modes found. ";
	}
	//find the best config if we're using multisampling
	int bestFbIndex = 0;
	if (format.samplesPerPixel > 1) {
		//pick the FB config/visual with the closest samples per pixel
		int bestDiff = 100;
		for (int i = 0; i < fbCount; i++ ) {
			XVisualInfo * visual = glXGetVisualFromFBConfig(xDisplay, fbConfig[i]);
			if (visual != nullptr) {
				int sampleBuffers = 0;
                int samples = 0;
				glXGetFBConfigAttrib(xDisplay, fbConfig[i], GLX_SAMPLE_BUFFERS, &sampleBuffers);
				glXGetFBConfigAttrib(xDisplay, fbConfig[i], GLX_SAMPLES, &samples);
				//check if this fits better
				int diff = abs(format.samplesPerPixel - samples);
				if (sampleBuffers == 1 && diff < bestDiff) {
					bestFbIndex = i;
					bestDiff = diff;
					if (diff == 0) {
						//prefect match. stop searching
						XFree(visual);
						break;
					}
				}
				XFree(visual);
			}
		}
	}
	//use best configuration found
	GLXFBConfig bestFbConfig = fbConfig[bestFbIndex];
	std::cout << "Using config #" << bestFbIndex << ":" << std::endl;
    int redBits = 0; glXGetFBConfigAttrib(xDisplay, bestFbConfig, GLX_RED_SIZE, &redBits );
    int greenBits = 0; glXGetFBConfigAttrib(xDisplay, bestFbConfig, GLX_GREEN_SIZE, &greenBits );
    int blueBits = 0; glXGetFBConfigAttrib(xDisplay, bestFbConfig, GLX_BLUE_SIZE, &blueBits );
    int alphaBits = 0; glXGetFBConfigAttrib(xDisplay, bestFbConfig, GLX_ALPHA_SIZE, &alphaBits );
    int sampleCount = 0; glXGetFBConfigAttrib(xDisplay, bestFbConfig, GLX_SAMPLES, &sampleCount );
    std::cout << "   Color buffer: R" << redBits << "G"  << greenBits << "B" << blueBits << "A" << alphaBits;
    if (sampleCount > 0)
    {
        std::cout << ", " << sampleCount << "x-multisampling" << std::endl;
    }
    else
    {
    std::cout << ", no multisampling" << std::endl;
    }
    int depthBits = 0; glXGetFBConfigAttrib(xDisplay, bestFbConfig, GLX_DEPTH_SIZE, &depthBits );
    int stencilBits = 0; glXGetFBConfigAttrib(xDisplay, bestFbConfig, GLX_STENCIL_SIZE, &stencilBits );
    std::cout << "   Depth buffer: D" << depthBits << "S"  << stencilBits << std::endl;
    int doubleBuffer = 0; glXGetFBConfigAttrib(xDisplay, bestFbConfig, GLX_DOUBLEBUFFER, &doubleBuffer );
    std::cout << "   " << (doubleBuffer ? "Double buffering" : "Single buffering") << std::endl;
    //std::cout << ": " << bestFbConfig-> << std::endl;
	//free the FBConfig list allocated by glXChooseFBConfig()
	XFree(fbConfig);
	//get a visual for that configuration
	XVisualInfo * visual = glXGetVisualFromFBConfig(xDisplay, bestFbConfig);

	//write our config into our format information
	int sampleBuffers, samples;
	glXGetFBConfigAttrib(xDisplay, bestFbConfig, GLX_SAMPLE_BUFFERS, &sampleBuffers);
	glXGetFBConfigAttrib(xDisplay, bestFbConfig, GLX_SAMPLES, &samples);
	format.samplesPerPixel = (sampleBuffers >= 1 ? samples : 1);

	//create window attributes
	Window xRootWindow = RootWindow(xDisplay, xScreenId);
	XSetWindowAttributes setWindowAttributes;
	setWindowAttributes.colormap = XCreateColormap(xDisplay, xRootWindow, visual->visual, AllocNone);
	setWindowAttributes.background_pixmap = None;
	setWindowAttributes.border_pixel = 0;
	setWindowAttributes.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | StructureNotifyMask;
	setWindowAttributes.override_redirect = False;
	unsigned long windowAttributes = CWBorderPixel | CWColormap | CWEventMask;

	//switch to fullscreen now if needed
	if (full) {
		setWindowAttributes.override_redirect = True;
		windowAttributes |= CWOverrideRedirect;
		XF86VidModeSwitchToMode(xDisplay, xScreenId, &bestMode);
		XF86VidModeSetViewPort(xDisplay, xScreenId, 0, 0);
	}
	//create actual window
	xWindow = XCreateWindow(xDisplay, xRootWindow, 0, 0, w, h, 0, visual->depth, InputOutput, visual->visual, windowAttributes, &setWindowAttributes);
	if (xWindow == 0) {
		//free visual data
		XFree(visual);
		destroy();
		std::cerr << "Failed to create an X11 window!" << std::endl;
		return;
	}
	//free visual data
	XFree(visual);
	//set window name
	XStoreName(xDisplay, xWindow, title.c_str());
	//show/map window and grab cursor
	if (full) {
		XWarpPointer(xDisplay, None, xWindow, 0, 0, 0, 0, 0, 0);
		XMapRaised(xDisplay, xWindow);
		XGrabKeyboard(xDisplay, xWindow, True, GrabModeAsync, GrabModeAsync, CurrentTime);
		XGrabPointer(xDisplay, xWindow, True, ButtonPressMask, GrabModeAsync, GrabModeAsync, xWindow, None, CurrentTime);
	}
	else {
		Atom wmDelete = XInternAtom(xDisplay, "WM_DELETE_WINDOW", True);
		XSetWMProtocols(xDisplay, xWindow, &wmDelete, 1);
		XSetStandardProperties(xDisplay, xWindow, title.c_str(), None, 0, nullptr, 0, nullptr);
		XMapRaised(xDisplay, xWindow);
	}
	//Create rendering context for it and activate it
	context = std::shared_ptr<GLContext>(new GLContext(xDisplay, xWindow, bestFbConfig));
	if (!context->isValid()) {
		destroy();
		std::cout << "Failed to create a render context!" << std::endl;
		return;
	}
	//set up some standard OpenGL stuff
	setup();
}

#endif

void GLWindow::setup()
{
	//make our context current
	context->makeCurrent();
	//set up view matrices and other OpenGL stuff
	GLdouble gldAspect = ((GLdouble)w/(GLdouble)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLdouble fovy = 30.0f;
	GLdouble zNear = 1.0f;
	GLdouble zFar = 10.0f;
	GLdouble ymax = 1.0f; //zNear * tan(fovy * PI / 360.0f);
	GLdouble ymin = -ymax;
	GLdouble xmin = ymin * gldAspect;
	GLdouble xmax = ymax * gldAspect;
	glFrustum(xmin, xmax, ymin, ymax, zNear, zFar);
	glViewport(0, 0, w, h);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void GLWindow::swap() const
{
#if defined(WIN32) || defined(_WIN32)
    SwapBuffers(hDC);
#elif defined(__linux__)
	glXSwapBuffers(xDisplay, xWindow);
#endif
}

void GLWindow::destroy()
{
	//free context
	context->destroy();
#if defined(WIN32) || defined(_WIN32)
	//switch back to old display mode
	if (full)	{
		if (ChangeDisplaySettings(&oldDisplayMode, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) {
			std::cout << "Mode switch to old mode failed. I'm sorry Dave." << std::endl;
		}
	}
	//Try to release the device context
	if (hDC != nullptr) {
		//release the DC we got for our window
		ReleaseDC(hWND, hDC);
		hDC = nullptr;
        DestroyWindow(hWND);
		hWND = nullptr;
	}
	//unhide cursor
	ShowCursor(true);
#elif defined(__linux__)
	if (xWindow != 0) {
		XDestroyWindow(xDisplay, xWindow);
		//XFreeColormap(xDisplay, xColorMap);
		if (full) {
			XF86VidModeSwitchToMode(xDisplay, xScreenId, &oldXMode);
			XF86VidModeSetViewPort(xDisplay, xScreenId, 0, 0);
		}
		XCloseDisplay(xDisplay);
	}
#endif
}

//common functions --------------------------------------------------------------------------------------------

const WindowBase::Format GLWindow::getDefaultFormat()
{
	return WindowBase::getDefaultFormat();
}

DisplayHandle GLWindow::getDisplayHandle() const
{
#if defined(WIN32) || defined(_WIN32)
	return hWND;
#elif defined(__linux__)
	return xDisplay;
#endif
}

void GLWindow::setSwapInterval(int interval) const
{
#if defined(WIN32) || defined(_WIN32)
	if (context->wglSwapInterval != nullptr) {
		context->wglSwapInterval(interval);
	}
#elif defined(__linux__)
    if (context->glXSwapInterval != nullptr) {
		context->glXSwapInterval(xDisplay, xWindow, interval);
	}
#endif
}

GLWindow::~GLWindow()
{
    destroy();
}
