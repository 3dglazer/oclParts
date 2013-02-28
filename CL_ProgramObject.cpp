<<<<<<< HEAD
=======
/*
 *  CL_ProgramObject.cpp
 *  GPUParticleSystem
 *
 *  Created by System Administrator on 1/30/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */
>>>>>>> cf3d2b62104e4d89c1b6556f45bbf9ded2905bce
#include "CL_Utils.h"
#include "CL_ProgramObject.h"

//constructor
CL_ProgramObject::CL_ProgramObject(){
	programObject=NULL;
	sourceCode=NULL;
}

CL_ProgramObject::CL_ProgramObject(CL_MainObject& clMO){
	clMainObj=&clMO;
	programObject=NULL;
	sourceCode=NULL;
}
//destructor
CL_ProgramObject::~CL_ProgramObject(){
	clReleaseProgram(programObject);
	free(sourceCode);
}

void CL_ProgramObject::loadFromSource(const char* cFilename){
	// Read the OpenCL kernel in from source file
	sourceCode = loadProgramSource(cFilename);
	// Create the program
	std::cout<<"Going to crate program "<<cFilename<<" ; with length "<<szFinalLength;
	
	for (int i=0; i<szFinalLength-1; i++) {
		std::cout << sourceCode[i];
	}
	
	programObject = clCreateProgramWithSource(clMainObj->context, 1, (const char **)&sourceCode, &szFinalLength, &(clMainObj->err));

	std::cout << "\n";
	std::cout<<cl_errString(clMainObj->err)<<"\n";
	clMainObj->err = clBuildProgram(programObject, 0, NULL, NULL, NULL, NULL);
	std::cout << "\n";
	std::cout<<cl_errString(clMainObj->err)<<"\n";
}


char* CL_ProgramObject::loadProgramSource(const char* cFilename) {
	// locals 
	FILE* pFileStream = NULL;
	size_t szSourceLength;
	
	
    pFileStream = fopen(cFilename, "rb");
    if(pFileStream == 0) {
		return NULL;
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
		return NULL;
	}
	
	// close the file and return the total length of the combined (preamble + source) string
	fclose(pFileStream);
	if(szFinalLength != NULL) {
		szFinalLength = szSourceLength;
	}
	cSourceString[szSourceLength] = '\0';
	
	return cSourceString;
}