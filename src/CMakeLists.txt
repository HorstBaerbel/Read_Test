cmake_minimum_required(VERSION 2.6)

project(read_test)

#-------------------------------------------------------------------------------
#add local find scripts to CMAKE path
LIST(APPEND CMAKE_MODULE_PATH
    ${CMAKE_CURRENT_SOURCE_DIR}/CMake/Utils
    ${CMAKE_CURRENT_SOURCE_DIR}/CMake/Packages
)

#-------------------------------------------------------------------------------
#set up OpenGL system variable
set(GLSystem "Desktop OpenGL" CACHE STRING "The OpenGL system to be used")
set_property(CACHE GLSystem PROPERTY STRINGS "Desktop OpenGL" "OpenGL ES 2.0")
#set up OpenGL window sytem variable
set(GLWindowSystem "" CACHE STRING "The OpenGL window system to be used")
set_property(CACHE GLWindowSystem PROPERTY STRINGS "WGL" "EGL" "GLX")

#-------------------------------------------------------------------------------
#check if we're running on Raspberry Pi
MESSAGE("Looking for bcm_host.h")
if(EXISTS "/opt/vc/include/bcm_host.h")
    MESSAGE("bcm_host.h found")
    set(BCMHOST found)
else()
    MESSAGE("bcm_host.h not found")
endif()

#-------------------------------------------------------------------------------
#Set OpenGL variables and compiler flags depending on system
if(DEFINED BCMHOST)
    set(GLSystem "OpenGL ES 2.0")
    set(GLWindowSystem "EGL")
    add_definitions(-DUSE_OPENGL_EGL)
    add_definitions(-DUSE_OPENGL_ES)
else()
    if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
        set(GLSystem "Desktop OpenGL")
        set(GLWindowSystem "WGL")
        add_definitions(-DUSE_OPENGL_WGL)
        add_definitions(-DUSE_OPENGL_DESKTOP)
    endif()
    if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        set(GLSystem "Desktop OpenGL")
        set(GLWindowSystem "GLX")
        add_definitions(-DUSE_OPENGL_GLX)
        add_definitions(-DUSE_OPENGL_DESKTOP)
    endif()
endif()

#-------------------------------------------------------------------------------
#finding necessary packages
if(${GLSystem} MATCHES "Desktop OpenGL")
    find_package(OpenGL REQUIRED)
else()
    find_package(OpenGLES2 REQUIRED)
endif()

#-------------------------------------------------------------------------------
#set up compiler flags and excutable names
if(DEFINED BCMHOST)
    add_definitions(-DBCMHOST)
endif()

if(MSVC)
    set(CMAKE_DEBUG_POSTFIX "d")
    add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
    add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP") #multi-processor compilation
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP") #multi-processor compilation
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
    #check for G++ 4.7+
    execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE G++_VERSION)
    if (G++_VERSION VERSION_LESS 4.7)
            message(SEND_ERROR "You need at least G++ 4.7 to compile!")
    endif()
    #set up compiler flags for GCC
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11") #support C++11
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")        #optimize
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")        #debugging
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s")  #strip binary
endif()

#-------------------------------------------------------------------------------
#add include directories
if(DEFINED BCMHOST)
    LIST(APPEND EXTRA_INCLUDE_DIRS
        "/opt/vc/include"
        "/opt/vc/include/interface/vcos"
        "/opt/vc/include/interface/vmcs_host/linux"
        "/opt/vc/include/interface/vcos/pthreads"
    )
else()
    if(${GLSystem} MATCHES "Desktop OpenGL")
        LIST(APPEND EXTRA_INCLUDE_DIRS
            ${OPENGL_INCLUDE_DIR}
        )
    else()
        LIST(APPEND EXTRA_INCLUDE_DIRS
            ${OPENGLES2_INCLUDE_DIR}
        )
    endif()
endif()

#-------------------------------------------------------------------------------
#define basic sources and headers
set(TARGET_HEADERS
    ContextBase.h
    GLFramebuffer.h
    GLTest.h
    GLTexture2D.h
    Test_glReadPixels.h
	Timing.h
    WindowBase.h
)
set(TARGET_SOURCES
    ContextBase.cpp
    GLFramebuffer.cpp
    GLTest.cpp
    GLTexture2D.cpp
    Test_glReadPixels.cpp
	Timing.cpp
    WindowBase.cpp
)

#-------------------------------------------------------------------------------
#define OS specific sources and headers
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    LIST(APPEND TARGET_SOURCES
        main_windows.cpp
    )
endif()
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    LIST(APPEND TARGET_SOURCES
        main_linux.cpp
    )
endif()

#add open gl specific sources
if(${GLSystem} MATCHES "Desktop OpenGL")
    LIST(APPEND TARGET_HEADERS
        GLContext.h
        GLWindow.h
        Test_glGetTexImage.h
        Test_PBO.h
    )
    LIST(APPEND TARGET_SOURCES
        GLContext.cpp
        GLWindow.cpp
        Test_glGetTexImage.cpp
        Test_PBO.cpp
    )
else()
    LIST(APPEND TARGET_HEADERS
        ESContext.h
        ESWindow.h
    )
    LIST(APPEND TARGET_SOURCES
        ESContext.cpp
        ESWindow.cpp
    )
endif()

#-------------------------------------------------------------------------------
#define libraries and directories
if(DEFINED BCMHOST)
#    link_directories(
#        ${CMAKE_CURRENT_BINARY_DIR}
#        "/opt/vc/lib"
#    )
    LIST(APPEND TARGET_LIBRARIES
        /opt/vc/lib/libbcm_host.so
        EGL
        ${OPENGLES2_LIBRARIES}
    )
else()
    if(MSVC)
        LIST(APPEND TARGET_LIBRARIES
            winmm
        )
    else()
        LIST(APPEND TARGET_LIBRARIES
            dl
            Xxf86vm
            X11
        )	
    endif()
    if(${GLSystem} MATCHES "Desktop OpenGL")
        LIST(APPEND TARGET_LIBRARIES
            ${OPENGL_LIBRARIES}
        )
    else()
        LIST(APPEND TARGET_LIBRARIES
            ${OPENGLES2_LIBRARIES}
        )
    endif()
endif()

#-------------------------------------------------------------------------------
#set up build directories
set(dir ${CMAKE_CURRENT_SOURCE_DIR}/../build)
set(EXECUTABLE_OUTPUT_PATH ${dir} CACHE PATH "Build directory" FORCE)
set(LIBRARY_OUTPUT_PATH ${dir} CACHE PATH "Build directory" FORCE)

#-------------------------------------------------------------------------------
#define target
include_directories(${EXTRA_INCLUDE_DIRS})
add_executable(read_test ${TARGET_SOURCES} ${TARGET_HEADERS})
target_link_libraries(read_test ${TARGET_LIBRARIES})

#special properties for windows builds
if(MSVC)
    #show console in debug builds, but not in proper release builds
    #Note that up to CMake 2.8.11.2 this feature is broken: http://public.kitware.com/Bug/view.php?id=12566
    set_target_properties(read_test PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE")
    set_target_properties(read_test PROPERTIES COMPILE_DEFINITIONS_DEBUG "_CONSOLE")
    set_target_properties(read_test PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:CONSOLE")
    set_target_properties(read_test PROPERTIES COMPILE_DEFINITIONS_RELWITHDEBINFO "_CONSOLE")
    set_target_properties(read_test PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
    set_target_properties(read_test PROPERTIES LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:WINDOWS")
endif()

