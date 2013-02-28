/*
 *  CL_MainObject.cpp
 *  GPUParticleSystem
 *
 *  Created by Zdenek Glazer on 1/10/11.
 *
 */

#include "CL_MainObject.h"


//Implicit constructor inits all to NULL
CL_MainObject::CL_MainObject()
{
	context=NULL;         // OpenCL context
	commandQueue=NULL;  // OpenCL command queue
	platformID=NULL;      // OpenCL platform
	deviceID=NULL;          // OpenCL device
	err=NULL;
}

//use this constructor if you want to create OpenCL context from OpenGL context
CL_MainObject::CL_MainObject(cl_device_type type,bool openGLShared){
	std::cout << "Aguire OpeCL platform"<<"\n";
	// 1. Get an OpenCL platform
	err = clGetPlatformIDs(1, &platformID, NULL);
	
	if (type==CL_DEVICE_TYPE_CPU) {
		if (openGLShared==true) {
			std::cout << "SORRY but CPU context can not be shared with OpenGL context, created standart one.";
		}
		context = clCreateContextFromType(0, 
										  CL_DEVICE_TYPE_CPU, 
										  NULL, 
										  NULL, 
										  &err);
	}else 
	{
		std::cout << "Aguire OpeCL device GPU"<<"\n";
		// 2. Get the devices
		err = clGetDeviceIDs(platformID, CL_DEVICE_TYPE_GPU, 1, &deviceID, NULL);
		
		if (openGLShared==true) {
			// 3. Create shared context context, beware osx specific code, this one works only on MAC OSX
			std::cout << "geting curent OpenGL context"<<"\n";
			CGLContextObj kCGLContext = CGLGetCurrentContext(); // GL Context
			CGLShareGroupObj kCGLShareGroup = CGLGetShareGroup(kCGLContext); // Share Group
			std::cout << "setting context properties"<<"\n";
			cl_context_properties props[] ={CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE,(cl_context_properties) kCGLShareGroup,CL_CONTEXT_PLATFORM,(cl_context_properties) platformID, 0};
			//this doesn't work, althought it's Apple's example code
			//context = clCreateContext(props, 1, *deviceID, NULL, NULL,&err);
			//misto clLogMessages muzu zkusit null 
			std::cout << "Creating OpenCL context";
			context = clCreateContext(props, 0, 0,clLogMessagesToStdoutAPPLE, NULL, &err);
		}else {
			// 3. Create the context
			context = clCreateContext(0, 1, &deviceID, clLogMessagesToStdoutAPPLE, NULL , &err);
		}
	}
	std::cout << "Creating OpenCL commandQue";
	// 4. Create a command-queue
	commandQueue = clCreateCommandQueue(context, deviceID, 0, &err);
	std::cout << "Ocl init done";
}

//destructor
CL_MainObject::~CL_MainObject(){
  //clReleaseCommandQueue(commandQueue);
  //clReleaseContext(context);
}