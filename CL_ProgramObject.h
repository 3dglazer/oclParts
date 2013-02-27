/*
 *  CL_ProgramObject.h
 *  GPUParticleSystem
 *
 *  Created by System Administrator on 1/30/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
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
