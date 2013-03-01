/*
 *  OpenCLObject.cpp
 *  GPUParticleSystem
 *
 * Created by Zdenek Glazer on 1/30/11.
 *
 */

#include "OpenCLObject.h"
#include "OpenGLUtils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <OpenCL/OpenCL.h>


#include "CL_EvenOddMergeSort.h"
#include "CL_MainObject.h"



#define EOL "\n"
using namespace std;
void pfn_notify(const char *errinfo, const void *private_info, size_t cb, void *user_data)
{
	//fprintf(stderr, "%s\n", errinfo);
}

OpenCLObject::OpenCLObject()
{
	cout << "Aguire OpeCL platform"<<EOL;
	// 1. Get an OpenCL platform
	err = clGetPlatformIDs(1, &platformID, NULL);
	checkError();
	
	cout << "Aguire OpeCL device"<<EOL;
	// 2. Get the devices
	err = clGetDeviceIDs(platformID, CL_DEVICE_TYPE_GPU, 1, &deviceID, NULL);
	checkError();
	
	// 3. Create shared context context, beware osx specific code, this one works only on MAC OSX
	cout << "geting curent opengl context"<<EOL;
	CGLContextObj kCGLContext = CGLGetCurrentContext(); // GL Context
	CGLShareGroupObj kCGLShareGroup = CGLGetShareGroup(kCGLContext); // Share Group
	cout << "setting context properties"<<EOL;
	cl_context_properties props[] ={CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE,(cl_context_properties) kCGLShareGroup,CL_CONTEXT_PLATFORM,(cl_context_properties) platformID, 0};
	//this doesn't work, althought it's Apple's example code
	//gpuContext = clCreateContext(props, 1, *deviceID, NULL, NULL,&err);
	//misto clLogMessages muzu zkusit null 
	cout << "Creating OpenCL context";

	gpuContext = clCreateContext(props, 0, 0,clLogMessagesToStdoutAPPLE, NULL, &err);

    checkError();
	
	cout << "Creating OpenCL commandQue";
	// 4. Create a command-queue
	commandQueue = clCreateCommandQueue(gpuContext, deviceID, 0, &err);
	checkError();
	
	cout << "Ocl init done";
}

//destructor
OpenCLObject::~OpenCLObject()
{
	if(kernelSourceCode)
		free(kernelSourceCode); 
	if(kernelObject)
		clReleaseKernel(kernelObject);  
	if(programObject)
		clReleaseProgram(programObject);
	if(commandQueue)
		clReleaseCommandQueue(commandQueue);
	if(gpuContext)
		clReleaseContext(gpuContext);
	exit (0);
}

void OpenCLObject::updateKernelArgs(){

}

void OpenCLObject::setupFields(){
	
	Fd_uniform fdUniform;
	Fd_drag fdDrag;
	Fd_radial fdRadial[2];
	Fd_vortex fdVortex;
	Fd_turbulence fdTurbulence;
	Fd_newton fdNewton;
	
	
	//seting uniform force field

	fdUniform.position=Vec4(0.0,0.0,0.0,0);
	fdUniform.magnitude=0.01;
	fdUniform.direction=Vec4(.1,.5,1.0,0);
	
	/*
	 //drag field
	 typedef struct {
	 Vec4 position; //attenuation is encoded in the 4. float value
	 Vec4 direction;  //maxDistance is encoded in direction.w
	 float magnitude;
	 float speedAttenuation; //stupid naming, means minimal particle speed to affect the particle.
	 }Fd_drag;
	 
	 //turbulence field
	 typedef struct {
	 Vec4 position; //attenuation is encoded in the 4. float value
	 Vec4 phase;  //maxDistance is encoded in phase.w
	 float magnitude;
	 float frequency; 
	 float noiseRatio;
	 int noiseLevel;
	 }Fd_turbulence;
	 
	 */
	
	//seting radial force field
	fdRadial[0].position=Vec4(.5,.5,-.5,1);
	fdRadial[0].magnitude=0.2;
	fdRadial[0].maxDistance=3;
	
	fdRadial[1].position=Vec4(-.5,.5,.5,2);
	fdRadial[1].magnitude=-0.2;
	fdRadial[1].maxDistance=3;
	
	//seting drag force field Nefunguje!!!
	fdDrag.position=Vec4(0,0,0,2);
	fdDrag.direction=Vec4(0,0,0,0);
	fdDrag.magnitude=0.1;
	fdDrag.speedAttenuation=0.1;
	//seting vortex
	fdVortex.position=Vec4(.0,0,0,1); //attenuation in w
	fdVortex.magnitude=0.2;
	fdVortex.axis=Vec4(0,0,1,0);//max distance in w
	//seting turbulence
	fdTurbulence.position=Vec4(.0,0,0,2);
	fdTurbulence.phase=Vec4(.0,0,0,0);
	fdTurbulence.magnitude=0.5;
	//fdTurbulence.frequency=0.5;
	fdTurbulence.frequency=4;
	fdTurbulence.noiseRatio=0.5;
	fdTurbulence.noiseLevel=1;
	
	fd_uniforms=clCreateBuffer(gpuContext, CL_MEM_READ_ONLY, sizeof(Fd_uniform), NULL, &err);
	fd_drags=clCreateBuffer(gpuContext, CL_MEM_READ_ONLY, sizeof(Fd_drag), NULL, &err);
	fd_radials=clCreateBuffer(gpuContext, CL_MEM_READ_ONLY, sizeof(Fd_radial)*2, NULL, &err);
	fd_vortices=clCreateBuffer(gpuContext, CL_MEM_READ_ONLY, sizeof(Fd_vortex), NULL, &err);
	fd_turbulences=clCreateBuffer(gpuContext, CL_MEM_READ_ONLY, sizeof(Fd_turbulence), NULL, &err);
	fd_newtons=clCreateBuffer(gpuContext, CL_MEM_READ_ONLY, sizeof(Fd_newton), NULL, &err);
	
	err = clEnqueueWriteBuffer(commandQueue, fd_uniforms, CL_TRUE, 0, sizeof(Fd_uniform), &fdUniform, 0, NULL, NULL);
	err = clEnqueueWriteBuffer(commandQueue, fd_drags, CL_TRUE, 0, sizeof(Fd_drag), &fdDrag, 0, NULL, NULL);
	err = clEnqueueWriteBuffer(commandQueue, fd_radials, CL_TRUE, 0, sizeof(Fd_radial)*2, &fdRadial, 0, NULL, NULL);
	err = clEnqueueWriteBuffer(commandQueue, fd_vortices, CL_TRUE, 0, sizeof(Fd_vortex), &fdVortex, 0, NULL, NULL);
	err = clEnqueueWriteBuffer(commandQueue, fd_turbulences, CL_TRUE, 0, sizeof(Fd_turbulence), &fdTurbulence, 0, NULL, NULL);
	err = clEnqueueWriteBuffer(commandQueue, fd_newtons, CL_TRUE, 0, sizeof(Fd_newton), &fdNewton, 0, NULL, NULL);
	
	err=clSetKernelArg(kernelObject, 7, sizeof(cl_mem), (void*)&fd_uniforms);
	err=clSetKernelArg(kernelObject, 8, sizeof(cl_mem), (void*)&fd_drags);
	err=clSetKernelArg(kernelObject, 9, sizeof(cl_mem), (void*)&fd_radials);
	err=clSetKernelArg(kernelObject, 10, sizeof(cl_mem), (void*)&fd_vortices);
	err=clSetKernelArg(kernelObject, 11, sizeof(cl_mem), (void*)&fd_turbulences);
	err=clSetKernelArg(kernelObject, 12, sizeof(cl_mem), (void*)&fd_newtons);
	
	
}

void OpenCLObject::checkError(){
	if (err != CL_SUCCESS) { 
		cout<<endl<<print_cl_errstring(err)<<endl; 
		//TADY BYCH MEL PROVEDST VYCISTENI PAMETI GRAFARNY ATD ABY NEBYLY MEMORY LEAKY
		//~OpenCLObject();     
	}
}

void  OpenCLObject::loadProgramSource(const char* cFilename) {
	// locals 
	FILE* pFileStream = NULL;
	size_t szSourceLength;
	
	
    pFileStream = fopen(cFilename, "rb");
    if(pFileStream == 0) {
		kernelSourceCode= NULL;
    }
	// get the length of the source code
	fseek(pFileStream, 0, SEEK_END); 
	szSourceLength = ftell(pFileStream);
	fseek(pFileStream, 0, SEEK_SET); 
	// allocate a buffer for the source code string and read it in
	char* cSourceString = (char *)malloc(szSourceLength + 1); 
	if (fread(cSourceString, szSourceLength, 1, pFileStream) != 1) {
		fclose(pFileStream);
		free(cSourceString);
		kernelSourceCode= NULL;
	}
	
	// close the file and return the total length of the combined (preamble + source) string
	fclose(pFileStream);
	

	programLength=szSourceLength;
	
	cSourceString[szSourceLength] = '\0';
	kernelSourceCode=cSourceString;

	cout<<	"source loaded\n";
}

void OpenCLObject::loadData(std::vector<Vec4> pos, std::vector<Vec4> vel, std::vector<Vec4> color){
	cout << "Creating vbos";
    num = pos.size();

	localWorkSize = 256;
	globalWorkSize = localWorkSize * ((int)((num+localWorkSize-1)/localWorkSize));

    array_size = num * sizeof(Vec4);
    //we have to create vertex buffer objects, for that we use utility from OpenGLUtils.h
    position_vbo = createVBO(&pos[0], array_size, GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW);
    color_vbo = createVBO(&color[0], array_size, GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW);
	cout << "vbos should be crated "<<endl;
    //OpenGL Context and OpenCL context must be synchronized that means, I have to take care about finishing them properly.
    glFinish();
	
	cout << "creating OpenGL vertex buffer objects"<<EOL;
	// create OpenCL buffer from GL VBO
    cl_pos_vbo=(clCreateFromGLBuffer(gpuContext,CL_MEM_READ_WRITE,position_vbo, &err));
    cl_col_vbo=(clCreateFromGLBuffer(gpuContext,CL_MEM_READ_WRITE,color_vbo, &err));
	
	//Create OpenCL Buffers, these can be accessed from the kernel code.
	cl_initialVelocities=clCreateBuffer(gpuContext, CL_MEM_READ_ONLY, array_size, NULL, &err); //initialvelocities
	cl_pos_buffer=clCreateBuffer(gpuContext, CL_MEM_READ_ONLY, array_size, NULL, &err);; //particle position buffer
	cl_vel_buffer=clCreateBuffer(gpuContext, CL_MEM_READ_ONLY, array_size, NULL, &err);; //particle velocity buffer
	
  	cout << "writing cpu data to gpu"<<EOL;
	
	err = clEnqueueWriteBuffer(commandQueue, cl_initialVelocities, CL_TRUE, 0, array_size, &vel[0], 0, NULL, NULL);
	checkError();
    err = clEnqueueWriteBuffer(commandQueue, cl_pos_buffer, CL_TRUE, 0, array_size, &pos[0], 0, NULL, NULL);
	checkError();
    err = clEnqueueWriteBuffer(commandQueue, cl_vel_buffer, CL_TRUE, 0, array_size, &vel[0], 0, NULL, NULL);
	checkError();
	cout << "date has been loaded";
	
	cout << "\n";
	cout << "going to create CL_MainObject";
	cout << "\n";
	
	CL_MainObject cm;
	cm.context=gpuContext;
	cm.deviceID=deviceID;
	cm.commandQueue=commandQueue;
	cm.platformID=platformID;
	cm.err=err;

	
	cout << "\n";
	cout << "CL_MainObject was Created";
	cout << "\n";

	/*
	CL_EvenOddMergeSort sort (cm);
	sort.sortFloats( cl_initialVelocities, 1024);
	
	float* test=new float[array_size ];
	for (int i=0; i<array_size; i++) {
		test[i] = 0;    // Initialize all elements to zero.
	}
	 */
	//clEnqueueReadBuffer(commandQueue, cl_initialVelocities, CL_TRUE, 0, array_size, test, 0, NULL, NULL);
	//cout << "after sort:"<<"/n";
	//for (int i=0; i<array_size; i++) {
		//cout<<test[i]<<" ";
		//cout << (float)vel[i].x<<" "<<(float)vel[i].y<<" "<<(float)vel[i].z<<" "<<(float)vel[i].w<<" ";
	//}
	
	//OpenGL Context and OpenCL context must be synchronized that means, I have to take care about finishing them properly.
    clFinish(commandQueue);
}


//for debug purpouses
char* OpenCLObject::print_cl_errstring(cl_int err) {
    switch (err) {
        case CL_SUCCESS:                          return strdup("Success!");
        case CL_DEVICE_NOT_FOUND:                 return strdup("Device not found.");
        case CL_DEVICE_NOT_AVAILABLE:             return strdup("Device not available");
        case CL_COMPILER_NOT_AVAILABLE:           return strdup("Compiler not available");
        case CL_MEM_OBJECT_ALLOCATION_FAILURE:    return strdup("Memory object allocation failure");
        case CL_OUT_OF_RESOURCES:                 return strdup("Out of resources");
        case CL_OUT_OF_HOST_MEMORY:               return strdup("Out of host memory");
        case CL_PROFILING_INFO_NOT_AVAILABLE:     return strdup("Profiling information not available");
        case CL_MEM_COPY_OVERLAP:                 return strdup("Memory copy overlap");
        case CL_IMAGE_FORMAT_MISMATCH:            return strdup("Image format mismatch");
        case CL_IMAGE_FORMAT_NOT_SUPPORTED:       return strdup("Image format not supported");
        case CL_BUILD_PROGRAM_FAILURE:            return strdup("Program build failure");
        case CL_MAP_FAILURE:                      return strdup("Map failure");
        case CL_INVALID_VALUE:                    return strdup("Invalid value");
        case CL_INVALID_DEVICE_TYPE:              return strdup("Invalid device type");
        case CL_INVALID_PLATFORM:                 return strdup("Invalid platform");
        case CL_INVALID_DEVICE:                   return strdup("Invalid device");
        case CL_INVALID_CONTEXT:                  return strdup("Invalid context");
        case CL_INVALID_QUEUE_PROPERTIES:         return strdup("Invalid queue properties");
        case CL_INVALID_COMMAND_QUEUE:            return strdup("Invalid command queue");
        case CL_INVALID_HOST_PTR:                 return strdup("Invalid host pointer");
        case CL_INVALID_MEM_OBJECT:               return strdup("Invalid memory object");
        case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:  return strdup("Invalid image format descriptor");
        case CL_INVALID_IMAGE_SIZE:               return strdup("Invalid image size");
        case CL_INVALID_SAMPLER:                  return strdup("Invalid sampler");
        case CL_INVALID_BINARY:                   return strdup("Invalid binary");
        case CL_INVALID_BUILD_OPTIONS:            return strdup("Invalid build options");
        case CL_INVALID_PROGRAM:                  return strdup("Invalid program");
        case CL_INVALID_PROGRAM_EXECUTABLE:       return strdup("Invalid program executable");
        case CL_INVALID_KERNEL_NAME:              return strdup("Invalid kernel name");
        case CL_INVALID_KERNEL_DEFINITION:        return strdup("Invalid kernel definition");
        case CL_INVALID_KERNEL:                   return strdup("Invalid kernel");
        case CL_INVALID_ARG_INDEX:                return strdup("Invalid argument index");
        case CL_INVALID_ARG_VALUE:                return strdup("Invalid argument value");
        case CL_INVALID_ARG_SIZE:                 return strdup("Invalid argument size");
        case CL_INVALID_KERNEL_ARGS:              return strdup("Invalid kernel arguments");
        case CL_INVALID_WORK_DIMENSION:           return strdup("Invalid work dimension");
        case CL_INVALID_WORK_GROUP_SIZE:          return strdup("Invalid work group size");
        case CL_INVALID_WORK_ITEM_SIZE:           return strdup("Invalid work item size");
        case CL_INVALID_GLOBAL_OFFSET:            return strdup("Invalid global offset");
        case CL_INVALID_EVENT_WAIT_LIST:          return strdup("Invalid event wait list");
        case CL_INVALID_EVENT:                    return strdup("Invalid event");
        case CL_INVALID_OPERATION:                return strdup("Invalid operation");
        case CL_INVALID_GL_OBJECT:                return strdup("Invalid OpenGL object");
        case CL_INVALID_BUFFER_SIZE:              return strdup("Invalid buffer size");
        case CL_INVALID_MIP_LEVEL:                return strdup("Invalid mip-map level");
        default:                                  return strdup("Unknown");
    }
} 
void OpenCLObject::initKernel(){
	cout << "Loading kernel source"<<endl;
	cout << "program length is "<<programLength<<endl;
	programObject = clCreateProgramWithSource(gpuContext, 1, (const char **)&kernelSourceCode, &programLength, &err);
	checkError();
	cout << "building kernel program"<<endl;
	err=clBuildProgram(programObject, 0, NULL, NULL, NULL, NULL);
	checkError();
	cout << "creating kernel Object";
	kernelObject = clCreateKernel(programObject, "part2", &err);
	checkError();
	cout << "seting arguments"<<endl;
	err=clSetKernelArg(kernelObject, 0, sizeof(cl_mem), (void*)&cl_pos_vbo);
	err=clSetKernelArg(kernelObject, 1, sizeof(cl_mem), (void*)&cl_col_vbo);
	err=clSetKernelArg(kernelObject, 2, sizeof(cl_mem), (void*)&cl_vel_buffer);
	err=clSetKernelArg(kernelObject, 3, sizeof(cl_mem), (void*)&cl_pos_buffer);
	err=clSetKernelArg(kernelObject, 4, sizeof(cl_mem), (void*)&cl_initialVelocities);
	
	//now i will setup fields that affect particles
	setupFields();
	
	checkError();
	//Wait for the command queue to finish these commands before proceeding

	clFinish(commandQueue);
	cout << "Arguments were succesfully set"<<endl;
	}
	
	
	
void OpenCLObject::runKernel(){
	//this will update our system by calculating new velocity and updating the positions of our particles
	//Make sure OpenGL is done using our VBOs
	//cout << "running kernel";

	glFinish();
	
	// map OpenGL buffer object for writing from OpenCL
	//this passes in the vector of VBO buffer objects (position and color)
	clEnqueueAcquireGLObjects(commandQueue, 1, &cl_pos_vbo, NULL, NULL, &event);
	clEnqueueAcquireGLObjects(commandQueue, 1, &cl_col_vbo, NULL, NULL, &event);
	//clWaitForEvents(1, &event);
	//cout << "vbos acquired from GL vbos";
	float dt = .001f;
	int nParts=num;
	err=clSetKernelArg(kernelObject, 5, sizeof(cl_float), &dt);
	err=clSetKernelArg(kernelObject, 6, sizeof(cl_int), &nParts);
	checkError();
	//execute the kernel
	//cout << "kernel is going to be executed";
	clEnqueueNDRangeKernel(commandQueue, kernelObject, 1, NULL,  &globalWorkSize, &localWorkSize, 0, NULL, &event);
	//clFinish(commandQueue);
	//cout << "Going to release vbos from CL";
	//Release the VBOs so OpenGL can play with them
	clEnqueueReleaseGLObjects(commandQueue, 1, &cl_pos_vbo, NULL, NULL, &event);
	clEnqueueReleaseGLObjects(commandQueue, 1, &cl_col_vbo, NULL, NULL, &event);
	clFinish(commandQueue);
	//cout << "vbos are again OpenGL's";
}