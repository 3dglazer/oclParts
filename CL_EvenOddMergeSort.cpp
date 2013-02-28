/*
 *  CL_EvenOddMergeSort.cpp
 *  GPUParticleSystem
 *
 *  Created by Zdenek Glazer on 1/30/11.
 *
 */
#include "CL_Utils.h"
#include "CL_EvenOddMergeSort.h"
CL_EvenOddMergeSort::CL_EvenOddMergeSort(){
	localWorkSize=512;
	//programObject.loadFromSource((const char*) "/myPrograms/GPUParticleSystem/evenOddMergeSort.cl");
}

CL_EvenOddMergeSort::CL_EvenOddMergeSort(CL_MainObject& clMO){
	localWorkSize=512;
	clMainObj=&clMO;
	programObject.clMainObj=&clMO;
	programObject.loadFromSource((const char*) "/myPrograms/GPUParticleSystem/evenOddMergeSort.cl");
	
}

CL_EvenOddMergeSort::~CL_EvenOddMergeSort(){

}
//EvenOddMerge sort works only on power of two!
int CL_EvenOddMergeSort::powerTwoSize(int desiredSize){
	int ret=0;
	int  powers[31]={0,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768,65536,131072,262144,524288,1048576,2097152,4194304,8388608,16777216,33554432,67108864,134217728,268435456,536870912,1073741824};
	for (int i=0; i<31; i++) {
		if(desiredSize<=powers[i]){ 
			ret=powers[i];
			break;
		}
	}
	return ret;
}
void CL_EvenOddMergeSort::initCLMainObject(){
	CL_MainObject cm (CL_DEVICE_TYPE_GPU,false);
	clMainObj=&cm;
}
void CL_EvenOddMergeSort::initKernelArgs(cl_mem &values,int n){
	iNumElements=powerTwoSize(n);
	globalWorkSize=iNumElements;
	clSetKernelArg(kernelObject, 0, sizeof(cl_mem), (void*)values);
	clSetKernelArg(kernelObject, 3, sizeof(cl_int), (void*)&n);
	
//	clSetKernelArg(kernelObject2, 0, sizeof(cl_mem), (void*)values);
//	clSetKernelArg(kernelObject2, 1, sizeof(cl_int)*localWorkSize, NULL);
//	clSetKernelArg(kernelObject2, 2, sizeof(cl_int), (void*)&n);
}
void CL_EvenOddMergeSort::initKernelArgs(cl_mem* keys,cl_mem* values,int n){
	iNumElements=powerTwoSize(n);
	globalWorkSize=iNumElements;
	clSetKernelArg(kernelObject, 0, sizeof(cl_mem), (void*)&keys);
	clSetKernelArg(kernelObject, 2, sizeof(cl_int), (void*)&iNumElements);
	clSetKernelArg(kernelObject, 4, sizeof(cl_int), (void*)&n);
	
	clSetKernelArg(kernelObject2, 0, sizeof(cl_mem), (void*)&keys);
	clSetKernelArg(kernelObject2, 1, sizeof(cl_mem), (void*)&values);
	clSetKernelArg(kernelObject2, 2, sizeof(cl_int)*localWorkSize, NULL);
	//well I use it even if cl_float specified, but the size of int and float are the same so it should work.
	clSetKernelArg(kernelObject2, 3, sizeof(cl_int)*localWorkSize, NULL);
	clSetKernelArg(kernelObject2, 4, sizeof(cl_int), (void*)&n);
}

void CL_EvenOddMergeSort::runKernel(){
	std::cout << "\n";
	std::cout << "going to run kernel object2";
	std::cout << "\n";
	std::cout<<cl_errString(clMainObj->err)<<"\n";
	clEnqueueNDRangeKernel((clMainObj->commandQueue), kernelObject2, 1, NULL, &globalWorkSize, &localWorkSize, 0, NULL, NULL);

	//clEnqueueNDRangeKernel((clMainObj->commandQueue), kernelObject2, 1, NULL, (size_t*)512, (size_t*)128, 0, NULL, NULL);
	std::cout << "\n";
	std::cout << "kernel2 runed succesfully";
	std::cout << "\n";
	std::cout << "global size is: "<<globalWorkSize<<"   Local size is: "<<localWorkSize;
	for(int d = 2; d <= iNumElements; d <<= 1){
		for(unsigned step = iNumElements / 2; step > 0; step >>= 1){
			clSetKernelArg(kernelObject, 1, sizeof(cl_int), (void*)&d);
			clSetKernelArg(kernelObject, 2, sizeof(cl_int), (void*)&step);
			clEnqueueNDRangeKernel((clMainObj->commandQueue), kernelObject, 1, NULL, &globalWorkSize, &localWorkSize, 0, NULL, NULL);
			//std::cout << cl_errString(clMainObj->err);
		}
	}
	std::cout << "\n";
	std::cout << "all kernels runed succesfully";
	std::cout << "\n";
}

void CL_EvenOddMergeSort::sortInts(cl_mem* values,int n){
	if (clMainObj==NULL) {
		initCLMainObject();
	}
	//set arguments
	kernelObject = clCreateKernel(programObject.programObject, "evenOddMergeSortIntVal", &(clMainObj->err));
	std::cout<<cl_errString(clMainObj->err);
	kernelObject2 = clCreateKernel(programObject.programObject, "evenOddMergeSortLocalMemoryIntVal", &(clMainObj->err));
	std::cout<<cl_errString(clMainObj->err);
	initKernelArgs(*values,  n);
	runKernel();
}
void CL_EvenOddMergeSort::sortFloats(cl_mem &values,int n){
	if (clMainObj==NULL) {
		initCLMainObject();
		std::cout << "shouldnot be here";
	}
	std::cout << "\n";
	std::cout << "going to create kernels";
	std::cout << "\n";
	//set arguments
	kernelObject = clCreateKernel(programObject.programObject, "evenOddMergeSortFloatVal", &(clMainObj->err));
	//kernelObject2 = clCreateKernel(programObject.programObject, "evenOddMergeSortLocalMemoryFloatVal", &(clMainObj->err));
	std::cout << cl_errString(clMainObj->err) ;
	std::cout << "\n";
	std::cout << "Kernels were created";
	std::cout << "\n";
	std::cout << "\n";
	std::cout << "going to set kernel args in sort";
	std::cout << "\n";
	initKernelArgs(values,  n);
	std::cout << "\n";
	std::cout << "args are set";
	std::cout << "\n";
	
	std::cout << "\n";
	std::cout << "going run kernels";
	std::cout << "\n";
	runKernel();
	std::cout << "\n";
	std::cout << "kernels succesfully ended";
	std::cout << "\n";
}

void CL_EvenOddMergeSort::sortInts(cl_mem* keys,cl_mem* values,int n){
	if (clMainObj==NULL) {
		initCLMainObject();
	}
	//set arguments
	kernelObject = clCreateKernel(programObject.programObject, "evenOddMergeSortKeysIntVal", &(clMainObj->err));
	kernelObject2 = clCreateKernel(programObject.programObject, "evenOddMergeSortLocalMemoryKeysIntVal", &(clMainObj->err));
	initKernelArgs(keys, values, n);
	runKernel();
}

void CL_EvenOddMergeSort::sortFloats(cl_mem* keys,cl_mem* values,int n){
	if (clMainObj==NULL) {
		initCLMainObject();
	}
	//set arguments
	kernelObject = clCreateKernel(programObject.programObject, "evenOddMergeSortKeysFloatVal", &(clMainObj->err));
	kernelObject2 = clCreateKernel(programObject.programObject, "evenOddMergeSortLocalMemoryKeysFloatVal", &(clMainObj->err));
	initKernelArgs(keys, values, n);
	runKernel();
}

void CL_EvenOddMergeSort::partialSortInts(cl_mem* keys,cl_mem* values,int n,int partNumber,int totalParts){
	if (clMainObj==NULL) {
		initCLMainObject();
	}
	//if n<=localWorkSize then there is no nead to divide it into iterations
	if (n<=localWorkSize) {
		sortInts(keys, values, n);
	}
	
}

void CL_EvenOddMergeSort::partialSortFloats(cl_mem* keys,cl_mem* values,int n,int partNumber,int totalParts){
	if (clMainObj==NULL) {
		initCLMainObject();
	}
	//if n<=localWorkSize then there is no nead to divide it into iterations
	if (n<=localWorkSize) {
		sortFloats(keys, values, n);
	}
	
}