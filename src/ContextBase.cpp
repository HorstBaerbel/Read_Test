#include "ContextBase.h"

#include <iostream>
#include <fstream>
#include <algorithm> //for std::transform

#if defined(__linux__)
	#include <dlfcn.h>
#endif

std::vector<ContextBase::Binding> ContextBase::bindings;


ContextBase::ContextBase()
	: versionMajor(1), versionMinor(0)
{
	if (bindings.size() <= 0) {
		//set up list with function pointers
		bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&glActiveTexture, "glActiveTexture"));
		bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&glCreateShader, "glCreateShader"));
		bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&glShaderSource, "glShaderSource"));
		bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&glCompileShader, "glCompileShader"));
		bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&glCreateProgram, "glCreateProgram"));
		bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&glAttachShader, "glAttachShader"));
		bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&glLinkProgram, "glLinkProgram"));
		bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&glUseProgram, "glUseProgram"));
		bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&glDetachShader, "glDetachShader"));
		bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&glDeleteShader, "glDeleteShader"));
		bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&glValidateProgram, "glValidateProgram"));
		bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&glGetUniformLocation, "glGetUniformLocation"));
		bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&glUniform1f, "glUniform1f"));
		bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&glUniform2f, "glUniform2f"));
		bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&glUniform3f, "glUniform3f"));
		bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&glUniform4f, "glUniform4f"));
		bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&glUniform1fv, "glUniform1fv"));
		bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&glUniform2fv, "glUniform2fv"));
		bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&glUniform3fv, "glUniform3fv"));
		bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&glUniform4fv, "glUniform4fv"));
		bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&glUniform1i, "glUniform1i"));
		bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&glUniform2i, "glUniform2i"));
		bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&glUniform3i, "glUniform3i"));
		bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&glUniform4i, "glUniform4i"));
		bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&glUniform1iv, "glUniform1iv"));
		bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&glUniform2iv, "glUniform2iv"));
		bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&glUniform3iv, "glUniform3iv"));
		bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&glUniform4iv, "glUniform4iv"));
		bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&glUniformMatrix2fv, "glUniformMatrix2fv"));
		bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&glUniformMatrix3fv, "glUniformMatrix3fv"));
		bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&glUniformMatrix4fv, "glUniformMatrix4fv"));
		bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&glGetAttribLocation, "glGetAttribLocation"));
		bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&glBindAttribLocation, "glBindAttribLocation"));
		bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&glVertexAttribPointer, "glVertexAttribPointer"));
		bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&glEnableVertexAttribArray, "glEnableVertexAttribArray"));
		bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&glDisableVertexAttribArray, "glDisableVertexAttribArray"));
		bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&glDrawArrays, "glDrawArrays"));
		bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&glDrawArrays, "glDrawArraysEXT"));
		bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&glDeleteProgram, "glDeleteProgram"));
		bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&glGetShaderiv, "glGetShaderiv"));
		bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&glGetShaderInfoLog, "glGetShaderInfoLog"));
		bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&glGetProgramiv, "glGetProgramiv"));
		bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&glGetProgramInfoLog, "glGetProgramInfoLog"));
		bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&glClampColor, "glClampColor"));
		bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&glGenFramebuffers, "glGenFramebuffers"));
		bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&glDeleteFramebuffers, "glDeleteFramebuffers"));
		bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&glBindFramebuffer, "glBindFramebuffer"));
		//bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&glFramebufferTexture, "glFramebufferTexture"));
		bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&glFramebufferTexture2D, "glFramebufferTexture2D"));
		bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&glBlitFramebuffer, "glBlitFramebuffer"));
		bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&glCheckFramebufferStatus, "glCheckFramebufferStatus"));
#ifdef USE_OPENGL_ES
		bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&glDiscardFramebuffer, "glDiscardFramebuffer"));
		bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&glDiscardFramebuffer, "glDiscardFramebufferEXT"));
#endif
		bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&glGenBuffers, "glGenBuffers"));
		bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&glBindBuffer, "glBindBuffer"));
		bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&glBufferData, "glBufferData"));
		bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&glMapBuffer, "glMapBuffer"));
		bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&glUnmapBuffer, "glUnmapBuffer"));
#if defined(WIN32) || defined(_WIN32)
		bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&wglGetSwapInterval, "wglGetSwapIntervalEXT"));
		bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&wglSwapInterval, "wglSwapIntervalEXT"));
		//try to find wglChoosePixelFormat as both regular and ARB function (needed for some systems)
		bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&wglChoosePixelFormat, "wglChoosePixelFormat"));
		bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&wglChoosePixelFormat, "wglChoosePixelFormatARB"));
#elif defined(__linux__)
	#ifdef USE_OPENGL_DESKTOP
		bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&glXSwapInterval, "glXSwapInterval"));
		bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&glXCreateContextAttribs, "glXCreateContextAttribs"));
        bindings.push_back(make_binding((void (GLAPIENTRYP*)(void))&glXCreateContextAttribs, "glXCreateContextAttribsARB"));
	#endif
#endif
	}
}

void ContextBase::getImplementationInfo()
{
    makeCurrent();
#ifdef USE_OPENGL_DESKTOP
	glGetIntegerv(GL_MAJOR_VERSION, &versionMajor);
	glGetIntegerv(GL_MINOR_VERSION, &versionMinor);
    auto versionString = glGetString(GL_VERSION);
    auto vendorString = glGetString(GL_VENDOR);
    auto rendererString = glGetString(GL_RENDERER);
    auto glslString = glGetString(GL_SHADING_LANGUAGE_VERSION);
	std::cout << "OpenGL version: " << versionString << std::endl;
	std::cout << "Vendor: " << vendorString << std::endl;
	std::cout << "Renderer: " << rendererString << std::endl;
	std::cout << "GLSL version: " << glslString << std::endl;
#endif
}

void ContextBase::getExtensions()
{
    makeCurrent();
#ifdef USE_OPENGL_DESKTOP
	glGetIntegerv(GL_MAJOR_VERSION, &versionMajor);
	//check opengl version
	if (versionMajor >= 3) {
		//opengl version starting with 3.0 should provide glGetStringi
#if defined(WIN32) || defined(_WIN32)
		PFNGLGETSTRINGIPROC glGetStringi = (PFNGLGETSTRINGIPROC)wglGetProcAddress("glGetStringi");
#elif defined(__linux__)
		#ifdef USE_OPENGL_DESKTOP
			PFNGLGETSTRINGIPROC glGetStringi = (PFNGLGETSTRINGIPROC)glXGetProcAddress((const GLubyte *)"glGetStringi");
		#else
			PFNGLGETSTRINGIPROC glGetStringi = (PFNGLGETSTRINGIPROC)eglGetProcAddress("glGetStringi");
		#endif
#endif
		if (glGetStringi != nullptr) {
			int numberOfExtensions = 0;
			glGetIntegerv(GL_NUM_EXTENSIONS, &numberOfExtensions);
			for (int i = 0; i < numberOfExtensions; i++) {
				const GLubyte * temp = glGetStringi(GL_EXTENSIONS, i);
				if (temp != nullptr) {
					extensions.push_back((const char *)temp);
				}
			}
		}
	}
#endif
	//if we haven't got extensions here, try the old method
	if (extensions.size() <= 0) {
		const GLubyte * temp = glGetString(GL_EXTENSIONS);
		if (temp != nullptr) {
			std::string wholeString((const char *)temp);
			std::string::size_type startOffset = 0;
			std::string::size_type endOffset = 0;
			std::string substring;
			do {
				substring.clear();
				endOffset = wholeString.find(' ', startOffset);
				if (endOffset != std::string::npos) {
					substring = wholeString.substr(startOffset, endOffset - startOffset);
					extensions.push_back(substring);
				}
				startOffset = endOffset + 1;
			} while(endOffset != std::string::npos);
		}
	}
	//check for errors
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
        std::cout << "Error 0x" << std::hex << error << " while getting extensions" << std::endl;
	}
    //dump extensions to stdout
    std::cout << "GL Extensions available: " << std::endl;
    std::vector<std::string>::const_iterator sit = extensions.begin();
    while(sit != extensions.end()) {
        std::cout << *sit << " ";
        sit++;
    }
    std::cout << std::endl;
}

bool ContextBase::makeCurrent()
{
    return false;
}

void ContextBase::destroy()
{
}

int ContextBase::getMajorVersion() const
{
	return versionMajor;
}

int ContextBase::getMinorVersion() const
{
	return versionMinor;
}

bool ContextBase::isExtensionAvailable(const std::string & extensionName) const
{
	//convert search string to lowercase
	std::string lower1 = extensionName;
	std::transform(lower1.begin(), lower1.end(), lower1.begin(), ::tolower);
	//search in extensions
    std::vector<std::string>::const_iterator sit = extensions.begin();
    while(sit != extensions.end()) {
		//convert extension to lowercase
		std::string lower0 = *sit;
		std::transform(lower0.begin(), lower0.end(), lower0.begin(), ::tolower);
		//compare the two
        if (lower0 == lower1) {
			return true;
		}
		sit++;
    }
	return false;
}

bool ContextBase::isDirect() const
{
	return false;
}

bool ContextBase::isValid() const
{
    return false;
}

ContextBase::Binding ContextBase::make_binding(void (GLAPIENTRYP * adressOfFunctionPointer)(), const char * nameOfFunction) const
{
    ContextBase::Binding b;
    b.adressOfFunctionPointer = adressOfFunctionPointer;
    b.nameOfFunction = nameOfFunction;
    return b;
}

bool ContextBase::getBindings()
{
	bool result = true;
	//clear all function pointers to null first
	std::vector<Binding>::const_iterator bit = bindings.cbegin();
    while (bit != bindings.cend()) {
		*(bit->adressOfFunctionPointer) = nullptr;
		++bit;
	}
	//try to get function pointer now. rewind iterator.
    bit = bindings.cbegin();
    while (bit != bindings.cend()) {
        //get function bindings here, depending on OS and OpenGL system
#if defined(WIN32) || defined(_WIN32)
		void * adress = (void *)wglGetProcAddress(bit->nameOfFunction);
#elif defined(__linux__)
		#ifdef USE_OPENGL_DESKTOP
			void * adress = (void *)glXGetProcAddress((const GLubyte *)bit->nameOfFunction);
		#else
			void * adress = (void *)eglGetProcAddress(bit->nameOfFunction);
		#endif
#endif
		if (adress != nullptr) {
			*(bit->adressOfFunctionPointer) = (void (GLAPIENTRYP)(void))adress;
		}
		else {
			result = false;
		}
        ++bit;
	}
#if defined(__linux__)
	//if we  get here in linux and there are still function pointers null try to load them again using dlopen/dlsym
	if (!result) {
		//try to open DLL
		void * dlHandle = dlopen(nullptr, RTLD_LAZY);
		if (dlHandle != nullptr) {
			//iterate through all bindings again. rewind iterator.
			bit = bindings.cbegin();
            while (bit != bindings.cend()) {
				//if the function pointer is still null, try to load it again
				if (*(bit->adressOfFunctionPointer) == nullptr) {
					//get symbol adress from DLL
					*(bit->adressOfFunctionPointer) = (void (GLAPIENTRYP)(void))dlsym(dlHandle, bit->nameOfFunction);
				}
				++bit;
			}
			//finally close DLL again
			dlclose(dlHandle);
		}
	}
#endif
	//now check all functions again for nullptrs. assume it all worked...
	result = true;
	//rewind iterator
    bit = bindings.cbegin();
    while (bit != bindings.cend()) {
		if (*(bit->adressOfFunctionPointer) == nullptr) {
			std::cout << "Failed to bind OpenGL function \"" << bit->nameOfFunction << "\"!" << std::endl;
			result = false;
		}
		++bit;
	}
	return result;
}

const std::string ContextBase::getShaderError(const GLuint vertexHandle, const GLuint fragmentHandle, const GLuint programHandle)
{
    //make context current first
    makeCurrent();
	//output infolog information about shader
    std::string info;
    //get length of info strings
    GLsizei vertexInfoLength = 0;
    GLsizei fragmentInfoLength = 0;
	GLsizei programInfoLength = 0;
    glGetShaderiv(vertexHandle, GL_INFO_LOG_LENGTH, &vertexInfoLength);
    glGetShaderiv(fragmentHandle, GL_INFO_LOG_LENGTH, &fragmentInfoLength);
    glGetProgramiv(programHandle, GL_INFO_LOG_LENGTH, &programInfoLength);
	if ((vertexInfoLength + fragmentInfoLength + programInfoLength) > 0) {
        //reserve space
        info.resize((vertexInfoLength + fragmentInfoLength + programInfoLength) + 100);
        //add info log strings
        GLsizei returned = 0;
        size_t currentSize = 0;//info.size();
        if (vertexInfoLength > 0) {
            glGetShaderInfoLog(vertexHandle, vertexInfoLength, &returned, &info[currentSize]);
            currentSize += vertexInfoLength;
            //info += "\n";
        }
        if (fragmentInfoLength > 0) {
            glGetShaderInfoLog(fragmentHandle, fragmentInfoLength, &returned, &info[currentSize]);
            currentSize += fragmentInfoLength;
            //info += "\n";
        }
        if (programInfoLength > 0) {
            glGetShaderInfoLog(programHandle, programInfoLength, &returned, &info[currentSize]);
            currentSize += programInfoLength;
            //info += "\n";
        }
	}
    return info;
}

GLuint ContextBase::createShader(const std::string & vertexCode, const std::string & fragmentCode, std::string & errorString)
{
    GLint result;
    GLuint vertexShader = 0;
    GLuint fragmentShader = 0;
    //make context current first
    makeCurrent();
    //create program
    GLuint programHandle = glCreateProgram();
    //successfully created?
    if (0 < programHandle) {
        //create vertex shader
        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        //successfully created?
        if (0 < vertexShader) {
            const GLchar * vertexData = (const GLchar *)vertexCode.data();
            glShaderSource(vertexShader, 1, &vertexData, 0);
            //compile vertex shader
            glCompileShader(vertexShader);
            //successfully compiled?
            glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
            if (GL_TRUE == result) {
                glAttachShader(programHandle, vertexShader);
                //create fragment shader
                fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
                //successfully created?
                if (0 < fragmentShader) {
                    const GLchar * fragmentData = (const GLchar *)fragmentCode.data();
                    glShaderSource(fragmentShader, 1, &fragmentData, 0);
                    //compile fragment shader
                    glCompileShader(fragmentShader);
                    //successfully compiled?
                    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
                    if (GL_TRUE == result) {
                        glAttachShader(programHandle, fragmentShader);
                        //link together
                        glLinkProgram(programHandle);
                        //successfully linked?
                        glGetProgramiv(programHandle, GL_LINK_STATUS, &result);
                        if (GL_TRUE == result) {
                            return programHandle;
                        }
                    }
                }
            }
        }
    }
    //get infolog
    errorString = getShaderError(vertexShader, fragmentShader, programHandle);
    //free all stuff created before
    if (0 != vertexShader) {
        glDetachShader(programHandle, vertexShader);
        glDeleteShader(vertexShader);
    }
    if (0 != fragmentShader) {
        glDetachShader(programHandle, fragmentShader);
        glDeleteShader(fragmentShader);
    }
    if (0 != programHandle) {
        glDeleteProgram(programHandle);
    }
    return 0;
}

GLuint ContextBase::createShaderFromFile(const std::string & vertexFile, const std::string & fragmentFile, std::string & errorString)
{
	GLuint shaderHandle = 0;
    std::ifstream vertexHandle;
    vertexHandle.open(vertexFile, std::ios::in | std::ios::binary);
    std::ifstream fragmentHandle;
    fragmentHandle.open(vertexFile, std::ios::in | std::ios::binary);
    if (vertexHandle.good() && fragmentHandle.good()) {
        //read vertex code
        std::string vertexCode;
        vertexHandle.seekg(0, std::ios::end);
        vertexCode.resize(vertexHandle.tellg());
        vertexHandle.seekg(0, std::ios::beg);
        vertexHandle.read(&vertexCode[0], vertexCode.size());
        vertexHandle.close();
        //read fragment code
        std::string fragmentCode;
        fragmentHandle.seekg(0, std::ios::end);
        fragmentCode.resize(fragmentHandle.tellg());
        fragmentHandle.seekg(0, std::ios::beg);
        fragmentHandle.read(&fragmentCode[0], fragmentCode.size());
        fragmentHandle.close();
        //create shaders
		shaderHandle = createShader(vertexCode, fragmentCode, errorString);
	}
	return shaderHandle;
}

ContextBase::~ContextBase()
{
    destroy();
}
