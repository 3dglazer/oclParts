/*
 *  CL_ProgramObject.h
 *  GPUParticleSystem
 *
 *  Created by Zdenek Glazer on 1/10/11.
 *
 */

#include "CL_Include.h"
#include "CL_MainObject.h"
class CL_ProgramObject {
private:
	size_t szFinalLength;
	char* sourceCode;
	char *loadProgramSource(const char* cFilename);
public:
	CL_MainObject* clMainObj;
	cl_program programObject;
	void loadFromSource(const char* cFilename);
	//constructor
	CL_ProgramObject();
	//constructor
	CL_ProgramObject(CL_MainObject& clMO);
	//destructor
	~CL_ProgramObject();
};
