/*
 *  CL_ProgramObject.h
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
