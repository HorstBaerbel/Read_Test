Read_Test
========

A benchmark for scaling and reading the OpenGL / OpenGL ES 1.x backbuffer to CPU memory. See my corresponding [blog page](http://lektiondestages.blogspot.de/2013/01/reading-opengl-backbuffer-to-system.html). 
It should work in Windows, Ubuntu and Raspbian using WGL, GLX and EGL.

License
========

The code is in the public domain. Do what you want with it, though mentioning where you had this from would be nice...

Building
========

**Use CMake:**
<pre>
cd Read_Test
cmake .
make
</pre>

GCC 4.7 is needed to compile. There is a CMake variable called `GLSystem` which you can set to either `Desktop OpenGL` or `OpenGL ES` for the system you want to use. On the Raspberry Pi it is automagically set to use OpenGL ES 1.x. You might also need OpenGL. If you don't have `/usr/include/GL/gl.h` and `libGL` try installing the MESA development package with:
```
sudo apt-get libgl1-mesa-dev
```

I found a bug or have suggestion
========

The best way to report a bug or suggest something is to post an issue on GitHub. Try to make it simple. You can also contact me via email if you want to.