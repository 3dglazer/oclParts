/*
 *  OpenGLUtils.h
 *  GPUEvenOddMergeSort
 *
 *  Created by System Administrator on 1/10/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef OPENGLUTILS_H
#define OPENGLUTILS_H
#include <OpenGL/OpenGL.h>



GLuint createVBO(const void* data, int dataSize, GLenum target, GLenum usage);
#endif