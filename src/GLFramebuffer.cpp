#include "GLFramebuffer.h"

#include <iostream>


#ifdef USE_OPENGL_ES
	GLuint GLFramebuffer::quadShader = 0;
	GLint GLFramebuffer::quadVertexAttribute = -1;
	GLint GLFramebuffer::quadTexCoordAttribute = -1;
	GLint GLFramebuffer::quadTexSamplerUniform = -1;
	const char * GLFramebuffer::vertexCode =
		"attribute vec4 vertex;\n"
		"attribute vec2 texCoord;\n"
		"varying vec2 vTexCoord;\n"
		"void main() {\n"
		"    gl_Position = vertex;\n"
		"    vTexCoord = texCoord;\n"
		"}\n";
	const char * GLFramebuffer::fragmentCode =
		"#ifdef GL_ES\n"
		"    precision mediump float;\n"
		"#endif\n"
		"varying vec2 vTexCoord;\n"
		"uniform sampler2D texSampler;\n"
		"void main() {\n"
		"    gl_FragColor = texture2D(texSampler, vTexCoord);\n"
		"}\n";
	float GLFramebuffer::quadVertices[] = {-1.0, -1.0, 0.0, 1.0, -1.0, 0.0, -1.0, 1.0, 0.0, 1.0, 1.0, 0.0};
	float GLFramebuffer::quadTexCoords[] = {0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f};
#endif


GLFramebuffer::GLFramebuffer(std::shared_ptr<ContextBase> & c)
	: context(c), glId(0)
{
	//make our context current
	context->makeCurrent();
#ifdef USE_OPENGL_ES
    //create statics on first run
    if (quadShader == 0) {
        std::string errorString;
        quadShader = context->createShader(vertexCode, fragmentCode, errorString);
        quadVertexAttribute = context->glGetAttribLocation(quadShader, "vertex");
        quadTexCoordAttribute = context->glGetAttribLocation(quadShader,"texCoord");
        quadTexSamplerUniform = context->glGetUniformLocation(quadShader, "texSampler");
    }
#endif
    //generate id for framebuffer object
    context->glGenFramebuffers(1, &glId);
	//check for errors
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
        std::cout << "Error 0x" << std::hex << error << " while creating GLFramebuffer" << std::endl;
	}
}

bool GLFramebuffer::attach(const GLenum attachmentPoint, std::shared_ptr<GLTexture2D> & texture)
{
	if (glId > 0) {
		//check if texture is valid
		if (!texture->isValid()) {
            std::cout << "Attachment of invalid GLTexture2D " << texture->getId() << " to point " << attachmentPoint << " not possible!" << std::endl;
			return false;
		}
		//if this is the first attachment, just use it, else check its size against the first one...
		if (attachments.size() == 0) {
			//alright. try attaching it
			context->glBindFramebuffer(GL_FRAMEBUFFER, glId);
			//the texture can not be bound when we try to attach it
			texture->unbind();
			context->glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentPoint, GL_TEXTURE_2D, texture->getId(), 0);
			//check for errors
			const GLenum error = glGetError();
			if (error != GL_NO_ERROR) {
                std::cout << "Attachment of GLTexture2D " << texture->getId() << " to point " << attachmentPoint << " failed with error 0x" << std::hex << error << "!" << std::endl;
				context->glBindFramebuffer(GL_FRAMEBUFFER, 0);
				return false;
			}
			//check if the framebuffer is complete
			const GLenum status = context->glCheckFramebufferStatus(GL_FRAMEBUFFER);
			if (GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT == status || GL_FRAMEBUFFER_UNSUPPORTED == status || GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS == status) {
                std::cout << "Attachment of GLTexture2D " << texture->getId() << " to point " << attachmentPoint << " failed. Framebuffer not complete. Error 0x" << std::hex << error << "!" << std::endl;
				context->glBindFramebuffer(GL_FRAMEBUFFER, 0);
				return false;
			}
			//unbind frame buffer
			context->glBindFramebuffer(GL_FRAMEBUFFER, 0);
			//just add it
			Attachment temp;
			temp.point = attachmentPoint;
			temp.texture = texture;
			attachments.push_back(temp);
			//add a discard element too
			discards.push_back(attachmentPoint);
			return true;
		}
		else {
			//check if texture sizes match
			if (texture->getWidth() != attachments.at(0).texture->getWidth() || texture->getHeight() != attachments.at(0).texture->getHeight()) {
                std::cout << "Attachment of GLTexture2D " << texture->getId() << " to point " << attachmentPoint << " not possible. Size is different!" << std::endl;
				return false;
			}
			//check if this is not attaching to the same point
			std::vector<Attachment>::const_iterator ait = attachments.cbegin();
			while(ait != attachments.cend()) {
				if (ait->point == attachmentPoint) {
					std::cout << "Attachment of GLTexture2D " << texture->getId() << " to point " << attachmentPoint << " not possible. Point already in use!" << std::endl;
					return false;
				}
			}
			//alright. try attaching it
			context->glBindFramebuffer(GL_FRAMEBUFFER, glId);
			//the texture can not be bound when we try to attach it
			texture->unbind();
			context->glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentPoint, GL_TEXTURE_2D, texture->getId(), 0);
			//check for errors
			const GLenum error = glGetError();
			if (error != GL_NO_ERROR) {
				std::cout << "Attachment of GLTexture2D " << texture->getId() << " to point " << attachmentPoint << " failed with error 0x" << std::hex << error << "!" << std::endl;
				context->glBindFramebuffer(GL_FRAMEBUFFER, 0);
				return false;
			}
			//check if the framebuffer is complete
			const GLenum status = context->glCheckFramebufferStatus(GL_FRAMEBUFFER);
			if (GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT == status || GL_FRAMEBUFFER_UNSUPPORTED == status || GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS == status) {
				std::cout << "Attachment of GLTexture2D " << texture->getId() << " to point " << attachmentPoint << " failed. Framebuffer not complete. Error 0x" << std::hex << status << "!" << std::endl;
				context->glBindFramebuffer(GL_FRAMEBUFFER, 0);
				return false;
			}
			//unbind frame buffer
			context->glBindFramebuffer(GL_FRAMEBUFFER, 0);
			//should have worked. store it
			Attachment temp;
			temp.point = attachmentPoint;
			temp.texture = texture;
			attachments.push_back(temp);
			//add a discard element too
			discards.push_back(attachmentPoint);
			return true;
		}
	}
	return false;
}

GLFramebuffer::Attachment GLFramebuffer::getAttachment(const GLenum attachmentPoint) const
{
	std::vector<Attachment>::const_iterator ait = attachments.cbegin();
	while(ait != attachments.cend()) {
		if (ait->point == attachmentPoint) {
			return (*ait);
		}
	}

	//TODO: This sucks! Do it right.
	return Attachment();
}

GLsizei GLFramebuffer::getWidth() const
{
	//return the width of the first attachment.
	if (glId > 0 && attachments.size() > 0) {
		return attachments.at(0).texture->getWidth();
	}
	return -1;
}

GLsizei GLFramebuffer::getHeight() const
{
	//return the height of the first attachment.
	if (glId > 0 && attachments.size() > 0) {
		return attachments.at(0).texture->getHeight();
	}
	return -1;
}

GLuint GLFramebuffer::getId() const
{
	return glId;
}

bool GLFramebuffer::isValid() const
{
	if (glId > 0) {
		if (attachments.size() > 0) {
			//bind framebuffer
			context->glBindFramebuffer(GL_FRAMEBUFFER, glId);
			//check for completeness
			const GLenum status = context->glCheckFramebufferStatus(GL_FRAMEBUFFER);
			if (GL_FRAMEBUFFER_COMPLETE == status) {
				//unbind it again
				context->glBindFramebuffer(GL_FRAMEBUFFER, 0);
				return true;
			}
			else {
                std::cout << "Framebuffer " << glId << " not complete. Error 0x" << std::hex << status << "!" << std::endl;
			}
			//unbind it again
			context->glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		else {
            std::cout << "Framebuffer " << glId << " has no attachments!" << std::endl;
		}
	}
	return false;
}

bool GLFramebuffer::blitTo(std::shared_ptr<GLFramebuffer> & to, const GLbitfield mask, const GLenum filter)
{
	if (glId > 0 && to->glId > 0) {
#ifdef USE_OPENGL_ES
		//bind the backbuffer for all operations
		context->glBindFramebuffer(GL_FRAMEBUFFER, to->glId);
		//set destination viewport size
		glViewport(0, 0, to->getWidth(), to->getHeight());
		//bind source texture
		attachments.front().texture->bind();
		context->glUseProgram(quadShader);
		context->glVertexAttribPointer(quadVertexAttribute, 3, GL_FLOAT, GL_FALSE, 0, quadVertices);
		context->glVertexAttribPointer(quadTexCoordAttribute, 2, GL_FLOAT, GL_FALSE, 0, quadTexCoords);
		context->glEnableVertexAttribArray(quadVertexAttribute);
		context->glEnableVertexAttribArray(quadTexCoordAttribute);
		context->glUniform1i(quadTexSamplerUniform, 0);
		context->glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
#else
		//bind this framebuffer for reading, the other for drawing
		context->glBindFramebuffer(GL_READ_FRAMEBUFFER, glId);
		context->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, to->glId);
		context->glBlitFramebuffer(0, 0, getWidth(), getHeight(), 0, 0, to->getWidth(), to->getHeight(), mask, filter);
#endif
		return true;
	}
	return false;
}

bool GLFramebuffer::blitToScreen(const int width, const int height, const GLbitfield mask, const GLenum filter)
{
	if (glId > 0) {
#ifdef USE_OPENGL_ES
		//bind the backbuffer for all operations
		context->glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//set destination viewport size
		glViewport(0, 0, width, height);
		//bind source texture
		attachments.front().texture->bind();
		context->glUseProgram(quadShader);
		context->glVertexAttribPointer(quadVertexAttribute, 3, GL_FLOAT, GL_FALSE, 0, quadVertices);
		context->glVertexAttribPointer(quadTexCoordAttribute, 2, GL_FLOAT, GL_FALSE, 0, quadTexCoords);
		context->glEnableVertexAttribArray(quadVertexAttribute);
		context->glEnableVertexAttribArray(quadTexCoordAttribute);
		context->glUniform1i(quadTexSamplerUniform, 0);
		context->glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
#else
		//bind this framebuffer for reading, the backbuffer for drawing
		context->glBindFramebuffer(GL_READ_FRAMEBUFFER, glId);
		context->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		context->glBlitFramebuffer(0, 0, getWidth(), getHeight(), 0, 0, width, height, mask, filter);
#endif
		return true;
	}
	return false;
}

bool GLFramebuffer::blitFromScreen(const int width, const int height, const GLbitfield mask, const GLenum filter)
{
#ifdef USE_OPENGL_ES
	//#error This function won't work in OpenGL ES.
#else
	if (glId > 0) {
		//bind this framebuffer for drawing, the backbuffer for reading
		context->glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		context->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, glId);
		context->glBlitFramebuffer(0, 0, width, height, 0, 0, getWidth(), getHeight(), mask, filter);
		return true;
	}
#endif
	return false;
}

bool GLFramebuffer::discard() const
{
    //only available on OpenGL ES (2.0)
#ifdef USE_OPENGL_ES
	if (glId > 0 && context->glDiscardFramebuffer != nullptr) {
		context->glBindFramebuffer(GL_FRAMEBUFFER, glId);
		context->glDiscardFramebuffer(GL_FRAMEBUFFER, discards.size(), discards.data());
		return true;
	}
#endif
	return false;
}

bool GLFramebuffer::bind() const
{
	if (glId > 0) {
		context->glBindFramebuffer(GL_FRAMEBUFFER, glId);
		return true;
	}
	return false;
}

bool GLFramebuffer::unbind() const
{
	context->glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return true;
}

GLFramebuffer::~GLFramebuffer()
{
	if (glId > 0) {
		//unbind and free framebuffer. attachments are freed automagically
		context->glBindFramebuffer(GL_FRAMEBUFFER, 0);
		context->glDeleteFramebuffers(1, &glId);
		glId = 0;
	}
}
