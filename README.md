Ocean-Breeze
=======================
Continuation of the [perlin noise generator](https://github.com/DarrenTsung/terrainator) project I made previously.
The sin-wave properties of the vertex are added on in the vertex shader (src/vshader1.vert) and the exercise was
to figure out how to load / debug / compile shaders and link and use a program with those shaders.
![gif](img/gif-1.gif?raw=true)

## Dependencies

 * OpenGL and GLUT.
 * Cmake.
 * Eigen.
 * C++ compiler with C++11 support.
 * Other dependencies are git submodules (glfw).

## Build
Install [Eigen](http://eigen.tuxfamily.org/) on your computer:
```
'cd path/to/unzippedEigen'
'mkdir build'
'cd build'
'cmake ..'
'make install'
```

Clone this repository and update the git submodules (`git submodule update`).

Just run `cmake` to build the Makefile, and then `make`. The executable will be
placed inside `bin` folder.

## Commands
 * up, down, left, right - rotate the model
 * SHIFT + up, down, left, right - translate the model

## Acknowledgments
 * [CMake](http://cmake.org) - cross-platform open-source build system.
 * [GLFW](http://www.glfw.org) - library for creating windows with OpenGL.
 * [Eigen](http://eigen.tuxfamily.org/) - library for linear algebra. 
 * http://webstaff.itn.liu.se/~stegu/TNM022-2005/perlinnoiselinks/perlin-noise-math-faq.html - great article on implementing perlin noise.
 * https://github.com/rodrigosetti/azteroids - used as reference for cmake and
   glfw, thanks!
 * http://www.davidcornette.com/glsl/glsl.html - copied shader code for phong shading and modified it, thanks!
