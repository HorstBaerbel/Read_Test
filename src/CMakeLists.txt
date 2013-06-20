cmake_minimum_required(VERSION 2.6)

project(read_test)

#include(CheckIncludeFiles)
#CHECK_INCLUDE_FILES("/opt/vc/include/bcm_host.h" BCMHOST)

find_package(OpenGL REQUIRED)

#set up OpenGL system variable
set(GLSystem "Desktop OpenGL" CACHE STRING "The OpenGL system to be used")
set_property(CACHE GLSystem PROPERTY STRINGS "Desktop OpenGL" "OpenGL ES")

#check if we're running on Raspberry Pi
MESSAGE("Looking for bcm_host.h")
if(EXISTS "/opt/vc/include/bcm_host.h")
    MESSAGE("bcm_host.h found")
    set(BCMHOST found)
    set(GLSystem "OpenGL ES")
    set(EXTRA_INCLUDE_DIRS "/opt/vc/include" "/opt/vc/include/interface/vcos" "/opt/vc/include/interface/vcos/pthreads")
else()
    MESSAGE("bcm_host.h not found")
endif()

#set up compiler flags and excutable names
if(DEFINED BCMHOST)
    add_definitions(-DBCMHOST)
endif()

if(MSVC)
    set(CMAKE_DEBUG_POSTFIX "d")
    add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
    add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-std=c++11")
#    set(CMAKE_CXX_FLAGS "-O2")        ## Optimize
#    set(CMAKE_EXE_LINKER_FLAGS "-s")  ## Strip binary
endif()

if(${GLSystem} MATCHES "Desktop OpenGL")
    add_definitions(-DUSE_OPENGL_DESKTOP)
else()
    add_definitions(-DUSE_OPENGL_ES)
endif()

#define basic sources
set(READ_TEST_HEADERS
    ContextBase.h
    GLFramebuffer.h
    GLTest.h
    GLTexture2D.h
    Test_glReadPixels.h
	Timing.h
    WindowBase.h
)
set(READ_TEST_SOURCES
    ContextBase.cpp
    GLFramebuffer.cpp
    GLTest.cpp
    GLTexture2D.cpp
    Test_glReadPixels.cpp
	Timing.cpp
    WindowBase.cpp
)

#add OS specifix sources
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    LIST(APPEND READ_TEST_SOURCES
        main_windows.cpp
    )
endif()
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    LIST(APPEND READ_TEST_SOURCES
        main_linux.cpp
    )
endif()

#add open gl specific sources
if(${GLSystem} MATCHES "Desktop OpenGL")
    LIST(APPEND READ_TEST_HEADERS
        GLContext.h
        GLWindow.h
        Test_glGetTexImage.h
        Test_PBO.h
    )
    LIST(APPEND READ_TEST_SOURCES
        GLContext.cpp
        GLWindow.cpp
        Test_glGetTexImage.cpp
        Test_PBO.cpp
    )
else()
    LIST(APPEND READ_TEST_HEADERS
        ESContext.h
        ESWindow.h
    )
    LIST(APPEND READ_TEST_SOURCES
        ESContext.cpp
        ESWindow.cpp
    )
endif()

if(${GLSystem} MATCHES "Desktop OpenGL")
    set(READ_TEST_LIBRARIES
        ${OPENGL_LIBRARIES}
    )
else()
    set(READ_TEST_LIBRARIES
        EGL
        GLESv2
    )
endif()

#define libraries and directories
if(DEFINED BCMHOST)
    LIST(APPEND READ_TEST_LIBRARIES
        bcm_host
    )
endif()

if(DEFINED BCMHOST)
    LINK_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR} "/opt/vc/lib")
endif()

set(dir ${CMAKE_CURRENT_SOURCE_DIR}/../build)
set(EXECUTABLE_OUTPUT_PATH ${dir} CACHE PATH "Build directory" FORCE)
set(LIBRARY_OUTPUT_PATH ${dir} CACHE PATH "Build directory" FORCE)

#define target
include_directories(${EXTRA_INCLUDE_DIRS})
add_executable(read_test ${READ_TEST_SOURCES} ${READ_TEST_HEADERS})
target_link_libraries(read_test ${READ_TEST_LIBRARIES})
