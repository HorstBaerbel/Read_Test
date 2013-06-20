#pragma once

#include "GLTest.h"
#include "GLFramebuffer.h"


class Test_glReadPixels : public GLTest
{
    GLint viewPort[4];

#ifdef USE_OPENGL_ES
    std::shared_ptr<GLFramebuffer> backbuffer;
	std::shared_ptr<GLTexture2D> backbufferTexture;
#endif

	unsigned char * downsampleData;
    std::shared_ptr<GLFramebuffer> downsample;
	std::shared_ptr<GLTexture2D> downsampleTexture;

public:
    Test_glReadPixels(std::shared_ptr<ContextBase> & c, const int width = GLTEST_DEFAULT_WIDTH, const int height = GLTEST_DEFAULT_HEIGHT, const int depth = GLTEST_DEFAULT_DEPTH);

    virtual std::string name() const;

	virtual bool pre();
	virtual bool post();

    ~Test_glReadPixels();
};