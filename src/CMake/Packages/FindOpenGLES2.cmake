#snapped from: https://bitbucket.org/sinbad/ogre/src/0bba4f7cdb95/CMake/Packages/FindOpenGLES.cmake?at=default
#-------------------------------------------------------------------
# This file is part of the CMake build system for OGRE
#     (Object-oriented Graphics Rendering Engine)
# For the latest info, see http://www.ogre3d.org/
#
# The contents of this file are placed in the public domain. Feel
# free to make use of it in any way you like.
#-------------------------------------------------------------------

# - Try to find OpenGLES 2.0
# Once done this will define
#  
#  OPENGLES2_FOUND        - system has OpenGLES 2.0
#  OPENGLES2_INCLUDE_DIR  - the GL include directory
#  OPENGLES2_LIBRARIES    - Link these to use OpenGLES 2.0

IF (WIN32)
  IF (CYGWIN)

    FIND_PATH(OPENGLES2_INCLUDE_DIR GLES2/gl2.h)

    FIND_LIBRARY(OPENGLES2_gl_LIBRARY libgles2)

  ELSE (CYGWIN)

    IF(MSVC)
      #The user hast to provide this atm. GLES2 can be emulated via Desktop OpenGL
      #using the ANGLE project found at: http://code.google.com/p/angleproject/
      SET (OPENGLES2_gl_LIBRARY import32 CACHE STRING "OpenGL ES 2.x library for Win32")
    ENDIF(MSVC)

  ENDIF (CYGWIN)

ELSE (WIN32)

  IF (APPLE)

        create_search_paths(/Developer/Platforms)
        findpkg_framework(OpenGLES)
    set(OPENGLES2_gl_LIBRARY "-framework OpenGLES2")

  ELSE(APPLE)

    FIND_PATH(OPENGLES2_INCLUDE_DIR GLES2/gl2.h
      /usr/openwin/share/include
      /opt/graphics/OpenGL/include /usr/X11R6/include
      /usr/include
      /opt/vc/include
    )

    FIND_LIBRARY(OPENGLES2_gl_LIBRARY
      NAMES GLES2 GLESv2
      PATHS /opt/graphics/OpenGL/lib
            /usr/openwin/lib
            /usr/shlib /usr/X11R6/lib
            /usr/lib
            /opt/vc/lib
    )

    # On Unix OpenGL most certainly always requires X11.
    # Feel free to tighten up these conditions if you don't 
    # think this is always true.

    IF (OPENGLES2_gl_LIBRARY)
      IF(NOT X11_FOUND)
        INCLUDE(FindX11)
      ENDIF(NOT X11_FOUND)
      IF (X11_FOUND)
        SET (OPENGLES2_LIBRARIES ${X11_LIBRARIES})
      ENDIF (X11_FOUND)
    ENDIF (OPENGLES2_gl_LIBRARY)

  ENDIF(APPLE)
ENDIF (WIN32)

SET( OPENGLES2_FOUND "NO" )
IF(OPENGLES2_gl_LIBRARY)

    SET( OPENGLES2_LIBRARIES ${OPENGLES2_gl_LIBRARY} ${OPENGLES2_LIBRARIES})

    SET( OPENGLES2_FOUND "YES" )

ENDIF(OPENGLES2_gl_LIBRARY)

MARK_AS_ADVANCED(
  OPENGLES2_INCLUDE_DIR
  OPENGLES2_gl_LIBRARY
)
