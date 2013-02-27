
void compareInt ( __global int* array, __private int posA,__private int posB, __constant int arrayLength){
	if(posA>arrayLength-1||posB>arrayLength-1){return ;}else{
		if(array[posA]>array[posB]){
			int t = array[posA];
			array[posA] = array[posB];
			array[posB] = t;
		}
	}
}


void compareFloat ( __global float* array, __private int posA,__private int posB, __constant int arrayLength){
	if(posA>arrayLength-1||posB>arrayLength-1){return ;}else{
		if(array[posA]>array[posB]){
			float t = array[posA];
			array[posA] = array[posB];
			array[posB] = t;
		}
	}
}

void compareIntSK ( __global int* key, __global int* value, __private int posA,__private int posB, __constant int arrayLength){
	if(posA>arrayLength-1||posB>arrayLength-1){return ;}else{
		if(key[posA]>key[posB]){
			int t = key[posA];
			key[posA] = key[posB];
			key[posB] = t;
		}
		if(value[posA]>value[posB]){
			int t = value[posA];
			value[posA] = value[posB];
			value[posB] = t;
		}
	}
}

void compareFloatSK ( __global int* key, __global float* value, __private int posA,__private int posB, __constant int arrayLength){
	if(posA>arrayLength-1||posB>arrayLength-1){return ;}else{
		if(key[posA]>key[posB]){
			int t = key[posA];
			key[posA] = key[posB];
			key[posB] = t;
		}
		if(value[posA]>value[posB]){
			float t = value[posA];
			value[posA] = value[posB];
			value[posB] = t;
		}
	}
}


//tento kernel vyuziva ciste global memory
__kernel void evenOddMergeSortIntVal( __global int* array,__constant int n,__constant int stride,__constant int arrayLength){
	int gid = get_global_id(0);	
    int arrayPosition= 2 * gid - (gid & (stride - 1)); //fast modulo opperation, works only on modulo 2
    if(stride < (n/2)){
		 int offset = gid & ( (n/2) - 1);
		 if(offset >= stride){
			 compareInt(array,arrayPosition-stride,arrayPosition,arrayLength);
		 }
	}
	else{
	compareInt(array,arrayPosition,arrayPosition+stride,arrayLength);
	}
}

__kernel void evenOddMergeSortFloatVal( __global float* array,__constant int n,__constant int stride,__constant int arrayLength){

	int gid = get_global_id(0);	
	
    int arrayPosition= 2 * gid - (gid & (stride - 1)); //fast modulo opperation, works only on modulo 2
	
    if(stride < (n/2)){
		 int offset = gid & ( (n/2) - 1);
		 if(offset >= stride){
			 compareFloat(array,arrayPosition-stride,arrayPosition,arrayLength);
		 }
	}
	else{
	compareFloat(array,arrayPosition,arrayPosition+stride,arrayLength);
	}
	
}

__kernel void evenOddMergeSortKeysIntVal( __global int* key,__global int* value,__constant int n,__constant int stride,__constant int arrayLength){
	int gid = get_global_id(0);	
    int arrayPosition= 2 * gid - (gid & (stride - 1)); //fast modulo opperation, works only on modulo 2
	
    if(stride < (n/2)){
		 int offset = gid & ( (n/2) - 1);
		 if(offset >= stride){
			 compareIntSK(key,value,arrayPosition-stride,arrayPosition,arrayLength);
		 }
	}
	else{
	compareIntSK(key,value,arrayPosition,arrayPosition+stride,arrayLength);
	}
}

__kernel void evenOddMergeSortKeysFloatVal( __global float* key,__global int* value,__constant int n,__constant int stride,__constant int arrayLength){
	int gid = get_global_id(0);	
    int arrayPosition= 2 * gid - (gid & (stride - 1)); //fast modulo opperation, works only on modulo 2
    if(stride < (n/2)){
		 int offset = gid & ( (n/2) - 1);
		 if(offset >= stride){
			 compareFloatSK(key,value,arrayPosition-stride,arrayPosition,arrayLength);
		 }
	}
	else{
		compareFloatSK(key,value,arrayPosition,arrayPosition+stride,arrayLength);
	}
}




void compareIntLocal(__local int* array, __private int posA,__private int posB, __constant int arrayLength){
	if(posA>arrayLength-1||posB>arrayLength-1){return ;}else{
		if(array[posA]>array[posB]){
			int t = array[posA];
			array[posA] = array[posB];
			array[posB] = t;
		}
	}
}
void compareFloatLocal(__local float* array, __private int posA,__private int posB, __constant int arrayLength){
	if(posA>arrayLength-1||posB>arrayLength-1){return ;}else{
		if(array[posA]>array[posB]){
			float t = array[posA];
			array[posA] = array[posB];
			array[posB] = t;
		}
	}
}

void compareIntLocalSK(__local int* key,__local int* value, __private int posA,__private int posB, __constant int arrayLength){
	if(posA>arrayLength-1||posB>arrayLength-1){return ;}else{
		if(key[posA]>key[posB]){
			int t = key[posA];
			key[posA] = key[posB];
			key[posB] = t;
		}
		if(value[posA]>value[posB]){
			int t = value[posA];
			value[posA] = value[posB];
			value[posB] = t;
		}
	}
}

void compareFloatLocalSK(__local int* key,__local float* value, __private int posA,__private int posB, __constant int arrayLength){
	if(posA>arrayLength-1||posB>arrayLength-1){return ;}else{
		if(key[posA]>key[posB]){
			int t = key[posA];
			key[posA] = key[posB];
			key[posB] = t;
		}
		if(value[posA]>value[posB]){
			float t = value[posA];
			value[posA] = value[posB];
			value[posB] = t;
		}
	}
}


//jenom Values tento kernel vyuziva shared memory
__kernel void evenOddMergeSortLocalMemoryIntVal(__global int* array, __local int* buffer,__constant int arrayLength){
	int lid=get_local_id(0);
	int gid= get_global_id(0);
	int localSize=get_local_size(0);
	
	buffer[lid]=array[gid];
	barrier(CLK_LOCAL_MEM_FENCE);
	
	int arrayPosition=0;
	for(int n = 2; n <= localSize; n <<= 1){
		for(unsigned stride = localSize / 2; stride > 0; stride >>= 1){
			arrayPosition= 2 * lid - (lid & (stride - 1)); //fast modulo opperation, works only on modulo 2
			if(stride < (n/2)){
				 int offset = lid & ( (n/2) - 1);
				 if(offset >= stride){
					 compareIntLocal(buffer,arrayPosition-stride,arrayPosition,arrayLength);
				 }
			}else{
			compareIntLocal(buffer,arrayPosition,arrayPosition+stride,arrayLength);
			}
			barrier(CLK_LOCAL_MEM_FENCE);
		}
	}
	array[gid]=buffer[lid];
}

//jenom Values tento kernel vyuziva shared memory
__kernel void evenOddMergeSortLocalMemoryFloatVal(__global float* array, __local int* buffer,__constant int arrayLength){
	int lid=get_local_id(0);
	int gid= get_global_id(0);
	int localSize=get_local_size(0);
	
	buffer[lid]=array[gid];
	barrier(CLK_LOCAL_MEM_FENCE);
	
	int arrayPosition=0;
	for(int n = 2; n <= localSize; n <<= 1){
		for(unsigned stride = localSize / 2; stride > 0; stride >>= 1){
			arrayPosition= 2 * lid - (lid & (stride - 1)); //fast modulo opperation, works only on modulo 2
			if(stride < (n/2)){
				 int offset = lid & ( (n/2) - 1);
				 if(offset >= stride){
					 compareFloatLocal(buffer,arrayPosition-stride,arrayPosition,arrayLength);
				 }
			}else{
			compareFloatLocal(buffer,arrayPosition,arrayPosition+stride,arrayLength);
			}
			barrier(CLK_LOCAL_MEM_FENCE);
		}
	}
	array[gid]=buffer[lid];
}

//tento a vzdy dvojice value a key kernel vyuziva shared memory
__kernel void evenOddMergeSortLocalMemoryKeysIntVal(__global int* key,__global int* value, __local int* bufferKey,__local int* bufferValue,__constant int arrayLength){
	int lid=get_local_id(0);
	int gid= get_global_id(0);
	int localSize=get_local_size(0);
	
	bufferValue[lid]=value[gid];
	bufferKey[lid]=key[gid];
	barrier(CLK_LOCAL_MEM_FENCE);
	
	int arrayPosition=0;
	for(int n = 2; n <= localSize; n <<= 1){
		for(unsigned stride = localSize / 2; stride > 0; stride >>= 1){
			arrayPosition= 2 * lid - (lid & (stride - 1)); //fast modulo opperation, works only on modulo 2
			if(stride < (n/2)){
				 int offset = lid & ( (n/2) - 1);
				 if(offset >= stride){
					 compareIntLocalSK(bufferKey,bufferValue,arrayPosition-stride,arrayPosition,arrayLength);
				 }
			}else{
			compareIntLocalSK(bufferKey,bufferValue,arrayPosition,arrayPosition+stride,arrayLength);
			}
			barrier(CLK_LOCAL_MEM_FENCE);
		}
	}
	key[gid]=bufferKey[lid];
	value[gid]=bufferValue[lid];
}

//tento a vzdy dvojice value a key kernel vyuziva shared memory
__kernel void evenOddMergeSortLocalMemoryKeysFloatVal(__global int* key,__global float* value, __local int* bufferKey,__local float* bufferValue,__constant int arrayLength){
	int lid=get_local_id(0);
	int gid= get_global_id(0);
	int localSize=get_local_size(0);
	
	bufferValue[lid]=value[gid];
	bufferKey[lid]=key[gid];
	barrier(CLK_LOCAL_MEM_FENCE);
	
	int arrayPosition=0;
	for(int n = 2; n <= localSize; n <<= 1){
		for(unsigned stride = localSize / 2; stride > 0; stride >>= 1){
			arrayPosition= 2 * lid - (lid & (stride - 1)); //fast modulo opperation, works only on modulo 2
			if(stride < (n/2)){
				 int offset = lid & ( (n/2) - 1);
				 if(offset >= stride){
					 compareFloatLocalSK(bufferKey,bufferValue,arrayPosition-stride,arrayPosition,arrayLength);
				 }
			}else{
			compareFloatLocalSK(bufferKey,bufferValue,arrayPosition,arrayPosition+stride,arrayLength);
			}
			barrier(CLK_LOCAL_MEM_FENCE);
		}
	}
	key[gid]=bufferKey[lid];
	value[gid]=bufferValue[lid];
}
/*
*/
