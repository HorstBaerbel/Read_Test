#include <iostream>

#ifdef USE_OPENGL_DESKTOP
	#include "GLWindow.h"
#else
    #include "ESWindow.h"
#endif

#include "Test_glReadPixels.h"
#ifdef USE_OPENGL_DESKTOP
	#include "Test_glGetTexImage.h"
	#include "Test_PBO.h"
#endif

#include "Timing.h"


bool showFPS = false;
bool doMultisampling = false;
int frames = 0;
GLTest * currentTest = nullptr;
GLTest * newTest = nullptr;
GLTest * tests[3];

std::shared_ptr<WindowBase> window;
std::shared_ptr<ContextBase> context;

GLuint triangleShader;
GLint triangleVertexAttribute;
GLint triangleColorAttribute;
const char * vertexCode =
    "attribute vec4 vertex;\n"
	"attribute vec4 color;\n"
    "varying vec4 col;\n"
	"void main() {\n"
	"    col = color;\n"
    "    gl_Position = vertex;\n"
    "}\n";
const char * fragmentCode =
    "#ifdef GL_ES\n"
	"    precision mediump float;\n"
    "#endif\n"
    "varying vec4 col;\n"
	"void main() {\n"
	"    gl_FragColor = col;\n"
	"}\n";
float triangleVertices[] = {-1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f};
float triangleColors[] = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};

void drawTriangle()
{
	//draw rgb triangle
    context->makeCurrent();
	context->glUseProgram(triangleShader);
	context->glVertexAttribPointer(triangleVertexAttribute, 3, GL_FLOAT, GL_FALSE, 0, triangleVertices);
	context->glVertexAttribPointer(triangleColorAttribute, 3, GL_FLOAT, GL_FALSE, 0, triangleColors);
	context->glEnableVertexAttribArray(triangleVertexAttribute);
	context->glEnableVertexAttribArray(triangleColorAttribute);
	context->glDrawArrays(GL_TRIANGLES, 0, 3);
}


#if defined(WIN32) || defined(_WIN32)

#endif

void testLoop(std::shared_ptr<ContextBase> & c, std::shared_ptr<WindowBase> & w, GLTest * test, const int mode)
{
    std::cout << "TEST: ";
    if (test) {
        test->setMode(mode);
        std::cout << test->name();
    }
    else {
        std::cout << "Default rendering";
    }
    std::cout << ", ";
    unsigned int startTicks = Timing::getTimeMsui();
    //draw loop
    do {
		//make context current
		context->makeCurrent();
        if (test) {
            test->pre();
        }
        else {
            //default: render to screen
			context->glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport(0, 0, w->getWidth(), w->getHeight());
        }
        //clear framebuffer
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		drawTriangle();
        if (test) {
            test->post();
        }
        //swap backbuffer to screen
        window->swap();
        //show frames/sec in title
        frames++;
        unsigned long endTicks = Timing::getTimeMsui();
        unsigned long tickDelta = endTicks - startTicks;
        if (tickDelta >= 3000) {
            std::cout.setf(std::ios::fixed, std::ios::floatfield);
            std::cout.precision(2);
            std::cout << "frame time " << (float)tickDelta / frames << " ms" << std::endl;
            startTicks = endTicks;
            frames = 0;
            break;
        }
#if defined(WIN32) || defined(_WIN32)
    } while(!GetAsyncKeyState(VK_ESCAPE));
#elif defined(__linux__)
	#ifdef USE_OPENGL_DESKTOP
	    //handle the events in the queue
	    const GLWindow * xwindow = dynamic_cast<GLWindow *>(window.get());
	    while (xwindow && XPending(xwindow->getDisplayHandle()) > 0) {
		    XEvent event;
		    XNextEvent(xwindow->getDisplayHandle(), &event);
		    if (event.type == KeyPress && XLookupKeysym(&event.xkey, 0) == XK_Escape) {
			    break;
		    }
	    }
	#endif
	} while(true);
#endif
}

bool setup()
{
	//create opengl window
#if defined(WIN32) || defined(_WIN32)
	LRESULT CALLBACK wndProc(HWND wnd, UINT message, WPARAM wParam, LPARAM lParam); //this is a "forward declaration" for the function defined in main_windows.cpp
	window = std::shared_ptr<GLWindow>(new GLWindow(640, 480, "read_test", false, GLWindow::getDefaultFormat(), (WNDPROC)wndProc));
#elif defined(__linux__)
    #ifdef USE_OPENGL_DESKTOP
		window = std::shared_ptr<GLWindow>(new GLWindow(640, 480, "read_test", false, GLWindow::getDefaultFormat()));
    #else
        window = std::shared_ptr<ESWindow>(new ESWindow(640, 480, "read_test", false, ESWindow::getDefaultFormat()));
    #endif
#endif
	window->setSwapInterval(0);
	context = window->getContext();
	//check if we have the needed extensions
/*	if (!context->isExtensionAvailable("GL_EXT_framebuffer_object") && !context->isExtensionAvailable("GL_ARB_framebuffer_object")) {
		std::cout << "Your system is lacking the extension GL_EXT_framebuffer_object. Exiting." << std::endl;
		return false;
	}
	if (!context->isExtensionAvailable("GL_EXT_framebuffer_blit") && !context->isExtensionAvailable("GL_ARB_framebuffer_blit")) {
		std::cout << "Your system is lacking the extension GL_EXT_framebuffer_blit. Exiting." << std::endl;
		return false;
	}
#ifdef USE_OPENGL_DESKTOP
	if (!context->isExtensionAvailable("GL_EXT_pixel_buffer_object") && !context->isExtensionAvailable("GL_ARB_pixel_buffer_object")) {
		std::cout << "Your system is lacking the extension GL_EXT_pixel_buffer_object. Exiting." << std::endl;
		return false;
	}
#endif
	if (!context->isExtensionAvailable("GL_ARB_texture_rectangle")) {
		std::cout << "Your system is lacking the extension GL_ARB_texture_rectangle. Exiting." << std::endl;
		return false;
	}
	if (!context->isExtensionAvailable("GL_ARB_texture_non_power_of_two")) {
		std::cout << "Your system is lacking the extension GL_ARB_texture_non_power_of_two. Exiting." << std::endl;
		return false;
	}*/
	//create tests
	tests[0] = new Test_glReadPixels(context);
#ifdef USE_OPENGL_DESKTOP
	tests[1] = new Test_glGetTexImage(context);
	tests[2] = new Test_PBO(context);
#endif
	//create shaders
	std::string errors;
	triangleShader = context->createShader(vertexCode, fragmentCode, errors);
	if (triangleShader > 0) {
		//get locations of vertex attributes
		triangleVertexAttribute = context->glGetAttribLocation(triangleShader, "vertex");
		triangleColorAttribute = context->glGetAttribLocation(triangleShader, "color");
	}
	else {
		std::cout << "Failed to create the necessary shader. Exiting." << std::endl;
		return false;
	}
	return true;
}

void benchmark()
{
	//do benchmark
	testLoop(context, window, nullptr, 0);
	testLoop(context, window, tests[0], 0);
	testLoop(context, window, tests[0], 1);
	testLoop(context, window, tests[0], 2);
#ifdef USE_OPENGL_DESKTOP
	testLoop(context, window, tests[1], 0);
	testLoop(context, window, tests[1], 1);
	testLoop(context, window, tests[1], 2);
	testLoop(context, window, tests[2], 0);
	testLoop(context, window, tests[2], 1);
	testLoop(context, window, tests[2], 2);
#endif
}

void interactive()
{
	//make context current
	context->makeCurrent();
	if (currentTest) {
		currentTest->pre();
	}
	else {
		//default: render to screen
		context->glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, window->getWidth(), window->getHeight());
	}
	//clear framebuffer
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	drawTriangle();
	if (currentTest) {
		currentTest->post();
	}
	//swap backbuffer to screen
	window->swap();
	//now change test if user wanted to
	currentTest = newTest;
}
