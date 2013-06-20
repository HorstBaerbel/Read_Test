#include "WindowBase.h"


WindowBase::WindowBase(const int width, const int height, std::string title, const bool fullScreen, const Format & windowFormat)
    : w(width), h(height), full(fullScreen), format(windowFormat)
{
}

void WindowBase::setup()
{
}

bool WindowBase::isFullscreen() const
{
    return full;
}

std::shared_ptr<ContextBase> & WindowBase::getContext()
{
    return context;
}

const WindowBase::Format & WindowBase::getFormat() const
{
	return format;
}

const WindowBase::Format WindowBase::getDefaultFormat()
{
	//standard format is RGBA8888, 24bit depth, no stencil, no multisampling, double buffering
	Format f = {8, 8, 8, 8, 24, 0, 1, true};
	return f;
}

DisplayHandle WindowBase::getDisplayHandle() const
{
	return nullptr;
}

void WindowBase::setSwapInterval(int interval) const
{
}

int WindowBase::getWidth() const
{
    return w;
}

int WindowBase::getHeight() const
{
    return h;
}

void WindowBase::swap() const
{
}

void WindowBase::destroy()
{
}

WindowBase::~WindowBase()
{
    destroy();
}
