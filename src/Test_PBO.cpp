#include "Test_PBO.h"

#include <iostream>


Test_PBO::Test_PBO(std::shared_ptr<ContextBase> & c, const int width, const int height, const int depth)
    : GLTest(c, width, height, depth), downsampleData(nullptr), readIndex(0), writeIndex(1)
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

    //create PBOs
    context->glGenBuffers(2, pbo);
    context->glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo[0]);
    context->glBufferData(GL_PIXEL_PACK_BUFFER, width * height * depth, 0, GL_STREAM_READ);
    context->glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo[1]);
    context->glBufferData(GL_PIXEL_PACK_BUFFER, width * height * depth, 0, GL_STREAM_READ);
    //unbind buffers for now
    context->glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
}

std::string Test_PBO::name() const
{
    if (mode == 1) {
        return "Read from PBOs asynchronously using glReadPixels. Reading GL_BGRA.";
    }
    else if (mode >= 2) {
        return "Read from PBOs asynchronously using glReadPixels. Reading GL_RGBA.";
    }
	return "Read from PBOs asynchronously using glReadPixels. Reading disabled.";
}

bool Test_PBO::pre()
{
	return GLTest::pre();
}

bool Test_PBO::post()
{
	GLTest::post();

	//blit framebuffer to downsampled buffer
    downsample->blitFromScreen(viewPort[2], viewPort[3]);

	if (mode > 0) {
		//bind downsampled fbo
		downsample->bind();
        //swap PBOs each frame
        writeIndex = (writeIndex + 1) % 2;
        readIndex = (writeIndex + 1) % 2;
        //bind PBO to read pixels
        context->glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo[writeIndex]);
		//read from framebuffer to PBO asynchronously
        if (mode == 1) {
		    glReadPixels(0, 0, downsample->getWidth(), downsample->getHeight(), GL_BGRA, GL_UNSIGNED_BYTE, nullptr);
        }
        else {
            glReadPixels(0, 0, downsample->getWidth(), downsample->getHeight(), GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        }
        //now read other PBO which should be ready
        context->glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo[readIndex]);
        //map buffer so we can access it
        downsampleData = (unsigned char *)context->glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
        if (downsampleData) {
            //worked. we have the memory now available to the CPU and could process it
            //...here...
            //unmap the buffer again
            downsampleData = nullptr;
            context->glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
        }
        //back to conventional pixel operation
        context->glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
	}

	//Comment this to see downsampling in action
	//downsample->blitToScreen(downsample->getWidth(), downsample->getHeight());

	downsample->unbind();

	return true;
}

Test_PBO::~Test_PBO()
{
}
