#include "GLTest.h"


GLTest::GLTest(std::shared_ptr<ContextBase> & c, const int width, const int height, const int depth)
	: context(c), w(width), h(height), d(depth), size(width*height*depth), mode(0)
{
	context->makeCurrent();
}

std::string GLTest::name() const
{
    return "";
}

void GLTest::setMode(const int value)
{
	mode = value;
}

int GLTest::getMode() const
{
    return mode;
}

bool GLTest::pre()
{
	context->makeCurrent();
	return true;
}

bool GLTest::post()
{
	context->makeCurrent();
	return true;
}