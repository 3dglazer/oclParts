#ifndef OPENGLUTILS_H
#define OPENGLUTILS_H
#include <OpenGL/OpenGL.h>

GLuint createVBO(const void* data, int dataSize, GLenum target, GLenum usage);
#endif