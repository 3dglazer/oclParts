<<<<<<< HEAD
=======
/*
 *  OpenGLUtils.cpp
 *  GPUEvenOddMergeSort
 *
 *  Created by System Administrator on 1/10/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

>>>>>>> cf3d2b62104e4d89c1b6556f45bbf9ded2905bce
#include "OpenGLUtils.h"
GLuint createVBO(const void* data, int dataSize, GLenum target, GLenum usage)
{
    GLuint id = 0; // 0 is reserved, glGenBuffersARB() will return non-zero id if success
	
    glGenBuffers(1, &id); // create a vbo
    glBindBuffer(target, id); // activate vbo id to use
    glBufferData(target, dataSize, data, usage); // upload data to video card
	
    // check data size in VBO is same as input array, if not return 0 and delete VBO
    int bufferSize = 0;
    glGetBufferParameteriv(target, GL_BUFFER_SIZE, &bufferSize);
    if(dataSize != bufferSize)
    {
        glDeleteBuffers(1, &id);
        id = 0;
        //cout << "[createVBO()] Data size is mismatch with input array\n";
    }
    //this was important for working inside blender!
    glBindBuffer(target, 0);
    return id; // return VBO id
}