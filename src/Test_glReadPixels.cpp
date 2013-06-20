#include "Test_glReadPixels.h"

#include <iostream>


Test_glReadPixels::Test_glReadPixels(std::shared_ptr<ContextBase> & c, const int width, const int height, const int depth)
    : GLTest(c, width, height, depth), downsampleData(nullptr)
{
	//get viewport size
	glGetIntegerv(GL_VIEWPORT, (GLint *)&viewPort);

#ifdef USE_OPENGL_ES
	//create full-sized framebuffer for OpenGL ES
	backbuffer = std::shared_ptr<GLFramebuffer>(new GLFramebuffer(c));
	//create texture for downsampled framebuffer and attach
	backbufferTexture = std::shared_ptr<GLTexture2D>(new GLTexture2D(context, viewPort[2], viewPort[3]));
	backbuffer->attach(GL_COLOR_ATTACHMENT0, backbufferTexture);
	if (!backbuffer->isValid()) {
        std::cout << "Backbuffer FBO not set up properly!" << std::endl;
        return;
    }
	else {
		std::cout << "Created " << viewPort[2] << "x" << viewPort[3] << "x" << depth*8 << " backbuffer." << std::endl;
	}
#endif

	//create downsampled framebuffer
	downsample = std::shared_ptr<GLFramebuffer>(new GLFramebuffer(c));
	//create texture for downsampled framebuffer and attach
	downsampleTexture = std::shared_ptr<GLTexture2D>(new GLTexture2D(context, width, height));
	downsample->attach(GL_COLOR_ATTACHMENT0, downsampleTexture);
	if (!downsample->isValid()) {
        std::cout << "Downsample FBO not set up properly!" << std::endl;
        return;
    }
	else {
		std::cout << "Created " << width << "x" << height << "x" << depth*8 << " downsampled framebuffer." << std::endl;
	}

	downsampleData = new unsigned char[width*height*depth];
}

std::string Test_glReadPixels::name() const
{
    if (mode == 1) {
        return "Read using glReadPixels. Reading GL_BGRA.";
    }
    else if (mode >= 2) {
        return "Read using glReadPixels. Reading GL_RGBA.";
    }
	return "Read using glReadPixels. Reading disabled.";
}

bool Test_glReadPixels::pre()
{
#ifdef USE_OPENGL_ES
	//set up backbuffer for rendering
	backbuffer->bind();
	//set proper viewport for backbuffer
	glViewport(0, 0, backbuffer->getWidth(), backbuffer->getHeight());
	return true;
#else
	return GLTest::pre();
#endif
}

bool Test_glReadPixels::post()
{
	GLTest::post();

#ifdef USE_OPENGL_ES
	//blit framebuffer to downsampled buffer
	backbuffer->blitToScreen(viewPort[2], viewPort[3]);
	backbuffer->blitTo(downsample);
#else
	//blit framebuffer to downsampled buffer
    downsample->blitFromScreen(viewPort[2], viewPort[3]);
#endif

	if (mode > 0) {
		//bind buffer for reading
		downsample->bind();
		//read from bound framebuffer to CPU
        if (mode == 1) {
			glReadPixels(0, 0, downsample->getWidth(), downsample->getHeight(), GL_BGRA, GL_UNSIGNED_BYTE, downsampleData);
		}
		else {
			glReadPixels(0, 0, downsample->getWidth(), downsample->getHeight(), GL_RGBA, GL_UNSIGNED_BYTE, downsampleData);
		}
	}

	//Uncomment this to see downsampling in action
	//downsample->blitToScreen(downsample->getWidth(), downsample->getHeight());

	downsample->unbind();

#ifdef USE_OPENGL_ES
	//reset viewport to whole screen
	glViewport(0, 0, viewPort[2], viewPort[3]);
#endif

	return true;
}

Test_glReadPixels::~Test_glReadPixels()
{
    delete [] downsampleData;
    downsampleData = nullptr;
}
