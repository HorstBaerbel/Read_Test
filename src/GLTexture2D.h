#pragma once

#include "ContextBase.h"


class GLTexture2D
{
	std::shared_ptr<ContextBase> context;
	GLuint glId;
	GLsizei w;
	GLsizei h;
	GLenum glInternalFormat;
	GLenum glFormat;
	GLenum glType;
	GLenum glUnit;
	bool valid;

public:
	GLTexture2D(std::shared_ptr<ContextBase> & c, const int width, const int height, const GLint internalFormat = GL_RGBA8, const GLenum format = GL_RGBA, const GLenum type = GL_UNSIGNED_BYTE);

	GLsizei getWidth() const;
	GLsizei getHeight() const;
	GLuint getId() const;
	GLenum getFormat() const;
	GLenum getType() const;
	bool isValid() const;

	bool setAutoMipMaps(const bool enable = false);
	bool setMagMinFilter(const GLenum magfilter = GL_LINEAR, const GLenum minfilter = GL_LINEAR);
	bool setWrapST(const GLenum wraps = GL_CLAMP_TO_EDGE, const GLenum wrapt = GL_CLAMP_TO_EDGE);
	bool setPixels(const GLvoid * pixels = nullptr, const GLint level = 0, const GLsizei width = -1, GLsizei height = -1);

	bool bind(GLenum unit = GL_TEXTURE0);
	bool unbind();

	~GLTexture2D();
};
