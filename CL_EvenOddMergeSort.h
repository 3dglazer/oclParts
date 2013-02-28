/*
 *  CL_EvenOddMergeSort.h
 *  GPUParticleSystem
 *
 *  Created by Zdenek Glazer on 1/10/11.
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