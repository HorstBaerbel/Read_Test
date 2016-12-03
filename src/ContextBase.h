#pragma once

#include <vector>
#include <string>
#include <memory>

#if defined(WIN32) || defined(_WIN32)
	#include <Windows.h>
#elif defined(__linux__)
	#include <X11/X.h>
	#include <X11/Xlib.h>
	#ifdef USE_OPENGL_DESKTOP
		#include <GL/glx.h>
	#endif
#endif

#ifdef USE_OPENGL_DESKTOP
	#include <GL/gl.h>
	#include <GL/glext.h>
#else
	#include <EGL/egl.h>
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
#endif

#ifdef USE_OPENGL_ES
	//Some GL headers lack some defines we use here...
	#define GL_READ_FRAMEBUFFER 0x8CA8
	#define GL_DRAW_FRAMEBUFFER 0x8CA9
	#define GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS 0x8DA8
	#define GL_BGRA 0x80E0
	#define GL_BGR 0x80E1
	#define GLAPIENTRYP *
#endif


//OpenGL context base class
class ContextBase
{
protected:
	int versionMajor; //!<OpenGL version information
	int versionMinor; //!<OpenGL version information
	std::vector<std::string> extensions;
    void getImplementationInfo();
	void getExtensions();

	struct Binding {
		void (GLAPIENTRYP * adressOfFunctionPointer)(); /*!<Pointer to where the function pointer is stored.*/
		const char * nameOfFunction;                    /*!<Name of the function we're trying to bind;*/
	};
    static std::vector<Binding> bindings; /*!<Holds adresses of OpenGL functions and the names.*/

    Binding make_binding(void (GLAPIENTRYP * adressOfFunctionPointer)(), const char * nameOfFunction) const;
	bool getBindings();

	ContextBase();

public:
	//Function pointers for stuff missing in Windows OpenGL interface
	void (GLAPIENTRYP glActiveTexture)(GLenum texture);                 /*!<Activate a specific texture unit*/
	//Function pointers to OpenGL shader functions.
	GLuint (GLAPIENTRYP glCreateShader)(GLenum shaderType);             /*!<Function prototype for shader creation.*/
	void (GLAPIENTRYP glShaderSource)(GLuint shader, GLsizei count, const GLchar **string, const GLint *length); /*!<Function prototype for shader source creation.*/
	void (GLAPIENTRYP glCompileShader)(GLuint shader);                /*!<Function prototype for shader compilation.*/
	GLuint (GLAPIENTRYP glCreateProgram)(void);                         /*!<Function prototype for shader program object creation.*/
	void (GLAPIENTRYP glAttachShader)(GLuint program, GLuint shader); /*!<Function prototype for attaching shader to program object.*/
	void (GLAPIENTRYP glLinkProgram)(GLuint program);                 /*!<Function prototype for shader compilation.*/
	void (GLAPIENTRYP glUseProgram)(GLuint program);                  /*!<Function prototype for using a shader program object.*/
	void (GLAPIENTRYP glDetachShader)(GLuint program, GLuint shader); /*!<Disconnect attached shader from program.*/
	void (GLAPIENTRYP glDeleteShader)(GLuint shader);                 /*!<Function prototype for deleting a shader source.*/
	void (GLAPIENTRYP glValidateProgram)(GLuint program);             /*!<Function prototype for validating a shader program.*/
	void (GLAPIENTRYP glDeleteProgram)(GLuint program);               /*!<Function prototype for deleting a shader program.*/
	void (GLAPIENTRYP glGetShaderiv)(GLuint shader, GLenum pname, GLint *params); /*!<Function prototype for getting parameters from a shader object.*/
	void (GLAPIENTRYP glGetShaderInfoLog)(GLuint shader, GLsizei maxLength, GLsizei *length, GLchar *infoLog); /*!<Function prototype to retrive infolog for a shader.*/
	void (GLAPIENTRYP glGetProgramiv)(GLuint program, GLenum pname, GLint *params); /*!<Function prototype for getting parameters from a program object.*/
	void (GLAPIENTRYP glGetProgramInfoLog)(GLuint program, GLsizei maxLength, GLsizei *length, GLchar *infoLog); /*!<Function prototype to retrive infolog for a program.*/
	//Function pointers to OpenGL shader uniform value functions.
	GLint (GLAPIENTRYP glGetUniformLocation)(GLuint program, const GLchar * name); /*!<Retrieve location of a uniform variable in a shader program.*/
	//float
	void (GLAPIENTRYP glUniform1f)(GLint location, GLfloat v0);                                     /*!<Set single uniform floating point value.*/
	void (GLAPIENTRYP glUniform2f)(GLint location, GLfloat v0, GLfloat v1);                         /*!<Set 2 uniform floating point values.*/
	void (GLAPIENTRYP glUniform3f)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);             /*!<Set 3 uniform floating point values.*/
	void (GLAPIENTRYP glUniform4f)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3); /*!<Set 4 uniform floating point values.*/
	void (GLAPIENTRYP glUniform1fv)(GLint location, GLsizei count, GLfloat *v);                     /*!<Set uniform floating point value vector.*/
	void (GLAPIENTRYP glUniform2fv)(GLint location, GLsizei count, GLfloat *v);                     /*!<Set uniform floating point value vector.*/
	void (GLAPIENTRYP glUniform3fv)(GLint location, GLsizei count, GLfloat *v);                     /*!<Set uniform floating point value vector.*/
	void (GLAPIENTRYP glUniform4fv)(GLint location, GLsizei count, GLfloat *v);                     /*!<Set uniform floating point value vector.*/
	//int
	void (GLAPIENTRYP glUniform1i)(GLint location, GLint v0);                               /*!<Set single uniform int value.*/
	void (GLAPIENTRYP glUniform2i)(GLint location, GLint v0, GLint v1);                     /*!<Set 2 uniform int values.*/
	void (GLAPIENTRYP glUniform3i)(GLint location, GLint v0, GLint v1, GLint v2);           /*!<Set 3 uniform int values.*/
	void (GLAPIENTRYP glUniform4i)(GLint location, GLint v0, GLint v1, GLint v2, GLint v3); /*!<Set 4 uniform int values.*/
	void (GLAPIENTRYP glUniform1iv)(GLint location, GLsizei count, GLint *v);               /*!<Set uniform int value vector.*/
	void (GLAPIENTRYP glUniform2iv)(GLint location, GLsizei count, GLint *v);               /*!<Set uniform int value vector.*/
	void (GLAPIENTRYP glUniform3iv)(GLint location, GLsizei count, GLint *v);               /*!<Set uniform int value vector.*/
	void (GLAPIENTRYP glUniform4iv)(GLint location, GLsizei count, GLint *v);               /*!<Set uniform int value vector.*/
	//matrix
	void (GLAPIENTRYP glUniformMatrix2fv)(GLint location, GLsizei count, GLboolean transpose, GLfloat *v); /*!<Set a 2x2 uniform floating point matrix.*/
	void (GLAPIENTRYP glUniformMatrix3fv)(GLint location, GLsizei count, GLboolean transpose, GLfloat *v); /*!<Set a 3x3 uniform floating point matrix.*/
	void (GLAPIENTRYP glUniformMatrix4fv)(GLint location, GLsizei count, GLboolean transpose, GLfloat *v); /*!<Set a 4x4 uniform floating point matrix.*/
	//vertex attributes and arrays
	GLint (GLAPIENTRYP glGetAttribLocation)(GLuint  program, const GLchar * name);
	void (GLAPIENTRYP glBindAttribLocation)(GLuint program, GLuint index, const GLchar * name);
	void (GLAPIENTRYP glVertexAttribPointer)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid * pointer);
	void (GLAPIENTRYP glEnableVertexAttribArray)(GLuint index);
	void (GLAPIENTRYP glDisableVertexAttribArray)(GLuint index);
	void (GLAPIENTRYP glDrawArrays)(GLenum mode, GLint first, GLsizei count);
	//FBOs
	void (GLAPIENTRYP glClampColor)(GLenum  target,  GLenum  clamp);
	void (GLAPIENTRYP glGenFramebuffers)(GLsizei n, GLuint *ids);
	void (GLAPIENTRYP glDeleteFramebuffers)(GLsizei n, const GLuint *ids);
	void (GLAPIENTRYP glBindFramebuffer)(GLenum target, GLuint id);
	//void (GLAPIENTRYP glFramebufferTexture)(GLenum target, GLenum attachment, GLuint texture, GLint level);
	void (GLAPIENTRYP glFramebufferTexture2D)(GLenum target, GLenum attachmentPoint, GLenum textureTarget, GLuint textureId, GLint  level); 
	void (GLAPIENTRYP glBlitFramebuffer)(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
	GLenum (GLAPIENTRYP glCheckFramebufferStatus)(GLenum target);
    void (GLAPIENTRYP glDiscardFramebuffer)(GLenum target, GLsizei numAttachments, const GLenum * attachments); //only on OpenGL ES (2.0)
	//VBOs, PBOs
	void (GLAPIENTRYP glGenBuffers)(GLsizei n, GLuint *ids);
	void (GLAPIENTRYP glBindBuffer)(GLenum target, GLuint id);
	void (GLAPIENTRYP glBufferData)(GLenum target, GLsizeiptr size, const GLvoid * data, GLenum usage);
	void * (GLAPIENTRYP glMapBuffer)(GLenum target, GLenum access);
	GLboolean (GLAPIENTRYP glUnmapBuffer)(GLenum target);

#if defined(WIN32) || defined(_WIN32)
	//wglGetExtensionsString();
	int (GLAPIENTRYP wglGetSwapInterval)(void);
	bool (GLAPIENTRYP wglSwapInterval)(int interval);
	bool (GLAPIENTRYP wglChoosePixelFormat)(HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);
#elif defined(__linux__)
	#ifdef USE_OPENGL_DESKTOP
		void (GLAPIENTRYP glXSwapInterval)(Display *dpy, GLXDrawable drawable, int interval);
		GLXContext (GLAPIENTRYP glXCreateContextAttribs)(Display *dpy, GLXFBConfig config, GLXContext share_context, Bool direct, const int *attrib_list);
    #endif
#endif

	virtual bool makeCurrent();
	virtual void destroy();

	int getMajorVersion() const;
	int getMinorVersion() const;
	bool isExtensionAvailable(const std::string & extensionName) const;
	virtual bool isDirect() const;

	virtual bool isValid() const;

	GLuint createShader(const std::string & vertexCode, const std::string & fragmentCode, std::string & errorString);
	GLuint createShaderFromFile(const std::string & vertexFile, const std::string & fragmentFile, std::string & errorString);
	const std::string getShaderError(const GLuint vertexHandle, const GLuint fragmentHandle, const GLuint programHandle);

    virtual ~ContextBase();
};
