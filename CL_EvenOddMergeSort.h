/*
 *  CL_EvenOddMergeSort.h
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
#ifndef CL_EVENODDMERGESORT_H
#define CL_EVENODDMERGESORT_H

#include "CL_Include.h"
#include "CL_MainObject.h"
#include "CL_ProgramObject.h"

class CL_EvenOddMergeSort{
	private:
	size_t globalWorkSize;          // 1D var for Total # of work items
	size_t localWorkSize;		    // 1D var for # of work items in the work group	
	int iNumElements;
	cl_kernel kernelObject,kernelObject2;         // OpenCL kernels
	CL_MainObject* clMainObj;
	CL_ProgramObject programObject;
	int powerTwoSize(int desiredSize);
	void initCLMainObject();
	void initKernelArgs(cl_mem &values,int n);
	void initKernelArgs(cl_mem* keys,cl_mem* values,int n);
	void runKernel();	
	public:	
	void sortInts(cl_mem* keys,cl_mem* values,int n);
	void sortFloats(cl_mem* keys,cl_mem* values,int n);
	
	void sortInts(cl_mem* values,int n);
	void sortFloats(cl_mem &values,int n);
	
	void partialSortInts(cl_mem* keys,cl_mem* values,int n,int partNumber,int totalParts);
	void partialSortFloats(cl_mem* keys,cl_mem* values,int n,int partNumber,int totalParts);
	
	CL_EvenOddMergeSort();
	CL_EvenOddMergeSort(CL_MainObject& clMO);
	~CL_EvenOddMergeSort();
	
};
#endif