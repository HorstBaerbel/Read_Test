#include "Test_glGetTexImage.h"

#include <iostream>


Test_glGetTexImage::Test_glGetTexImage(std::shared_ptr<ContextBase> & c, const int width, const int height, const int depth)
    : GLTest(c, width, height, depth), downsampleData(nullptr)
{
	//get viewport size
	glGetIntegerv(GL_VIEWPORT, (GLint *)&viewPort);

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

std::string Test_glGetTexImage::name() const
{
    if (mode == 1) {
        return "Read using glGetTexImage. Reading GL_BGRA.";
    }
    else if (mode >= 2) {
        return "Read using glGetTexImage. Reading GL_RGBA.";
    }
	return "Read using glGetTexImage. Reading disabled.";
}

bool Test_glGetTexImage::pre()
{
	return GLTest::pre();
}

bool Test_glGetTexImage::post()
{
	GLTest::post();

    //blit framebuffer to downsampled buffer
    downsample->blitFromScreen(viewPort[2], viewPort[3]);

	if (mode > 0) {
        //bind downsampled texture
        downsampleTexture->bind();
        //read from bound texture to CPU
        if (mode == 1) {
	        glGetTexImage(GL_TEXTURE_2D, 0, GL_BGRA, GL_UNSIGNED_BYTE, downsampleData);
        }
        else {
            glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, downsampleData);
        }
		//unbind texture again
		downsampleTexture->unbind();
	}

	//Uncomment this to see downsampling in action
	//downsample->blitToScreen(downsample->getWidth(), downsample->getHeight());

	downsample->unbind();

	return true;
}

Test_glGetTexImage::~Test_glGetTexImage()
{
    delete [] downsampleData;
    downsampleData = nullptr;
}
