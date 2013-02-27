/*
 *  OpenCLObject.h
 *  GPUParticleSystem
 *
 *  Created by System Administrator on 1/10/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef CL_OPENCLOBJECT_H
#define CL_OPENCLOBJECT_H
#include <OpenCL/OpenCL.h>
#include <vector>

// issue with using cl_float4 from cl_platform.h
// http://www.khronos.org/message_boards/viewtopic.php?f=28&t=1848

typedef struct Vec4
{
    float x,y,z,w;
    Vec4(){};
    Vec4(float xx, float yy, float zz, float ww):
	x(xx),
	y(yy),
	z(zz),
	w(ww)
    {}
    void set(float xx, float yy, float zz, float ww=1.) {
        x = xx;
        y = yy;
        z = zz;
        w = ww;
    }
} Vec4 __attribute__((aligned(16)));






//========================== Force Fields	=============================
//All these structures are also part of the opencl code, the only difference is these use Vec4 instead of float4
//All the names of the forceFields are equivalent to maya force fields.

//newton field
typedef struct {
	Vec4 position; //attenuation is encoded in the 4. float value
	float magnitude;
	float maxDistance;
	float minDistance;
	float mass;
	//float attenuation; it's encoded in the pos.w 
} Fd_newton;

//radial field
typedef struct {
	Vec4 position; //attenuation is encoded in the 4. float value
	float magnitude;
	float maxDistance;
	//float attenuation; it's encoded in the pos.w 
} Fd_radial;

//uniform field
typedef struct {
	Vec4 position; //attenuation is encoded in the 4. float value
	float magnitude;
	Vec4 direction; //maxDistance is encoded in direction.w
	//float maxDistance;
} Fd_uniform;

//vortex field
typedef struct {
	Vec4 position; //attenuation is encoded in the 4. float value
	float magnitude;
	Vec4 axis; //maxDistance is encoded in axis.w
} Fd_vortex;

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

//========================== /Force Fields	=============================









class OpenCLObject {
	
private:	

	
	cl_context gpuContext;          // OpenCL context
	cl_command_queue commandQueue;  // OpenCL command queue
	cl_platform_id platformID;      // OpenCL platform
	cl_device_id deviceID;          // OpenCL device
	
	cl_program programObject;
	size_t programLength; //loaded program length
	cl_kernel kernelObject;
	
	cl_int err; //error codes
	cl_event  event;
	//checks errors after every OpenCL command. 
	char* kernelSourceCode;         // Buffer to hold source for compilation 
	
	void checkError();
	//for debug purpouses
	char* print_cl_errstring(cl_int err);
	void setupFields();
public:
	//these variables are aplication dependent, can't be reused in other implementations
	//--------aplication dependent--------------
	cl_mem cl_initialVelocities; //particle velocities
	cl_mem cl_pos_buffer; //particle position buffer
	cl_mem cl_vel_buffer; //particle velocity buffer
	cl_mem cl_pos_vbo;
	cl_mem cl_col_vbo;
	
	//fields in constant memory
	cl_mem	fd_uniforms;		//7
	cl_mem	fd_drags;			//8
	cl_mem	fd_radials;			//9
	cl_mem	fd_vortices;		//10
	cl_mem	fd_turbulences;		//11	
	cl_mem	fd_newtons;			//12
	
	int position_vbo; //position vbo
	int color_vbo; //colors vbo
	int num; //the number of particles
	
	size_t globalWorkSize;          // 1D var for Total # of work items
	size_t localWorkSize;			// local work size
	size_t array_size; //the size of our arrays num * sizeof(Vec4)

	//setup the data for the kernel
	void loadData(std::vector<Vec4> pos, std::vector<Vec4> vel, std::vector<Vec4> color);
	void initKernel();
	//////////aplication dependent///////////////
	//load an OpenCL program from a string
	void loadProgramSource(const char* cFilename);
	//execute the kernel
	void runKernel();
	
	//constructor
	OpenCLObject();
	//destructor
	~OpenCLObject();
	



};
#endif