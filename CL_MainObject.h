/*
 *  CL_MainObject.h
 *  GPUParticleSystem
 *
<<<<<<< HEAD
 *  Created by Zdenek Glazer on 1/10/11.
=======
 *  Created by System Administrator on 1/30/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
>>>>>>> cf3d2b62104e4d89c1b6556f45bbf9ded2905bce
 *
 */
#ifndef CL_MAINOBJECT_H
#define CL_MAINOBJECT_H
#include "CL_Include.h"


class CL_MainObject {

	public:	
		cl_context context;          // OpenCL context
		cl_command_queue commandQueue;  // OpenCL command queue
		cl_platform_id platformID;      // OpenCL platform
		cl_device_id deviceID;          // OpenCL device
		cl_int err; //error codes
		//Implicit constructor inits all to NULL
		CL_MainObject();
		//use this constructor if you want to create OpenCL context from OpenGL context
		CL_MainObject(cl_device_type type,bool openGLShared);
		//destructor
		~CL_MainObject();
};
#endif