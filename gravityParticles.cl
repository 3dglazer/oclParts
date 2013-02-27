//========================== Helper functions ===========================
//sums up two vectors, operates on float4, althought sums only first three componets.
inline float4 vec4_add3(float4 a, float4 b){
a.x+=b.x;
a.y+=b.y;
a.z+=b.z;
return a;
}

// returns b-a.
inline float4 vec4_sub3(float4 a,float4 b){
b.x-=a.x;
b.y-=a.y;
b.z-=a.z;
return b;
}
//========================== /Helper functions ==========================

//========================== Uniform Grid ===============================
//large prime numbers
#define P1 73856093
#define P2 19349663
#define P3 83492791

//jak vyresit uniform grid?
//1) hash funkce particle to grid
//2) seradit gridCellId (vzdy dvojice gcd a particleId)
//3) castice musi vedet do kteryho gridu patri, takze musi vedet kam sahat do pole??
//4) paralel prefix sum??

//pole ktere je velke jako pocet gridCells
//gridcell vi pocatecni index sveho gridu v pole pro particles


//tohle je pole pro particles:
//000000001155555558889999
//12345678...particle ids
//particles potrebuji vedet alespon start sveho gridCellu v poli

//inline uint calcGridHash(float4 position,uint numCells)
//{
//uint n = (uint)(P1*pow(((uint)position.x),P2)*pow(((uint)position.y) , P3)*((uint)position.z));
//n = n%numCells;
//return n;
//}



//========================== /Uniform Grid ===============================

//======================= 3D Noise function =======================

// this noise function code is based on the apple example code.
// original code can be found at http://developer.apple.com/library/mac/#samplecode/OpenCL_Procedural_Noise_Example/Listings/noise_kernel_cl.html

__constant int P_MASK = 255;
__constant int P_SIZE = 256;
__constant int P[512] = {0, 103, 125, 109, 88, 213, 57, 222, 243, 248, 21, 239, 169, 93, 37, 135, 244, 146, 203,
160, 182, 149, 54, 75, 51, 63, 242, 163, 5, 165, 45, 180, 157, 81, 83, 147, 206, 100, 91, 58, 168, 129, 43, 106,
77, 108, 126, 223, 124, 140, 138, 52, 62, 123, 208, 67, 254, 87, 60, 181, 104, 19, 69, 224, 247, 161, 189, 191,
166, 0, 231, 164, 221, 212, 196, 39, 32, 227, 92, 73, 174, 240, 119, 26, 107, 102, 197, 177, 66, 46, 96, 49,
226, 178, 18, 198, 249, 233, 11, 9, 8, 167, 150, 194, 134, 101, 218, 153, 98, 55, 185, 95, 33, 36, 156, 97, 122,
2, 17, 211, 1, 175, 24, 234, 176, 121, 7, 232, 195, 105, 170, 20, 190, 241, 225, 230, 216, 50, 219, 201, 148,
144, 154, 71, 151, 192, 118, 207, 186, 14, 200, 114, 56, 53, 179, 99, 84, 70, 31, 113, 40, 30, 131, 128, 110,
217, 86, 16, 183, 159, 65, 64, 214, 80, 158, 152, 143, 38, 236, 127, 48, 250, 187, 162, 94, 210, 132, 29, 171,
79, 68, 199, 133, 15, 252, 142, 6, 82, 245, 145, 112, 28, 139, 34, 173, 35, 255, 253, 220, 193, 130, 12, 90,
47, 41, 111, 184, 3, 202, 188, 115, 42, 251, 215, 136, 59, 22, 155, 13, 10, 120, 44, 137, 23, 141, 74, 238,
85, 72, 246, 205, 89, 61, 237, 204, 78, 27, 4, 116, 229, 228, 209, 117, 25, 235, 172, 0, 109, 179, 77, 214,
232, 212, 60, 133, 40, 244, 146, 106, 64, 225, 219, 125, 97, 36, 230, 99, 93, 81, 63, 245, 241, 86, 5, 236,
118, 19, 255, 190, 130, 128, 47, 6, 100, 96, 12, 113, 111, 52, 14, 192, 188, 54, 37, 17, 196, 105, 95, 227, 
180, 166, 153, 144, 123, 102, 29, 21, 223, 210, 193, 101, 87, 26, 8, 240, 197, 178, 165, 154, 104, 76, 18, 
253, 239, 224, 148, 84, 31, 0, 254, 252, 217, 207, 202, 177, 147, 134, 129, 90, 62, 53, 233, 213, 199, 187, 
176, 158, 122, 65, 22, 10, 226, 203, 186, 181, 175, 137, 136, 66, 1, 243, 234, 220, 216, 215, 208, 167, 149, 
145, 141, 127, 119, 98, 80, 74, 68, 43, 42, 200, 189, 170, 115, 103, 83, 55, 49, 38, 35, 7, 242, 206, 198, 
184, 182, 135, 120, 112, 108, 94, 79, 75, 73, 72, 69, 30, 251, 246, 231, 205, 191, 164, 150, 143, 126, 92, 
67, 61, 44, 34, 28, 24, 23, 250, 249, 237, 211, 163, 157, 121, 107, 89, 88, 82, 78, 70, 45, 39, 32, 27, 16, 
15, 11, 4, 3, 2, 248, 238, 204, 201, 183, 174, 168, 159, 151, 116, 114, 110, 58, 50, 41, 33, 229, 228, 222, 
221, 195, 173, 161, 160, 139, 138, 132, 131, 124, 117, 91, 85, 25, 13, 9, 247, 235, 218, 209, 194, 185, 172, 
171, 169, 162, 156, 155, 152, 142, 140, 71, 59, 57, 56, 51, 48, 20, 
  };
  
__constant int G_MASK = 15;
__constant int G_SIZE = 16;
__constant int G_VECSIZE = 4;
__constant float G[16*4] = {
	  +1.0f,  +1.0f, +0.0f, +0.0f, 
	  -1.0f,  +1.0f, +0.0f, +0.0f, 
	  +1.0f,  -1.0f, +0.0f, +0.0f, 
	  -1.0f,  -1.0f, +0.0f, +0.0f,
	  +1.0f, +0.0f,  +1.0f, +0.0f, 
	  -1.0f, +0.0f,  +1.0f, +0.0f, 
	  +1.0f, +0.0f,  -1.0f, +0.0f, 
	  -1.0f, +0.0f,  -1.0f, +0.0f,
	 +0.0f,  +1.0f,  +1.0f, +0.0f, 
	 +0.0f,  -1.0f,  +1.0f, +0.0f, 
	 +0.0f,  +1.0f,  -1.0f, +0.0f, 
	 +0.0f,  -1.0f,  -1.0f, +0.0f,
	  +1.0f,  +1.0f, +0.0f, +0.0f, 
	  -1.0f,  +1.0f, +0.0f, +0.0f, 
	 +0.0f,  -1.0f,  +1.0f, +0.0f, 
	 +0.0f,  -1.0f,  -1.0f, +0.0f
};  

float smooth(float t)
{
	return t*t*t*(t*(t*6.0f-15.0f)+10.0f); 
}

float mix1d(float a, float b, float t)
{
	float ba = b - a;
	float tba = t * ba;
	float atba = a + tba;
	return atba;	
}

float2 mix2d(float2 a, float2 b, float t)
{
	float2 ba = b - a;
	float2 tba = t * ba;
	float2 atba = a + tba;
	return atba;	
}

float4 mix3d(float4 a, float4 b, float t)
{
	float4 ba = b - a;
	float4 tba = t * ba;
	float4 atba = a + tba;
	return atba;	
}

int lattice3d(int4 i)
{
	return P[i.x + P[i.y + P[i.z]]];
}

float gradient3d(int4 i, float4 v)
{
	int index = (lattice3d(i) & G_MASK) * G_VECSIZE;
	float4 g = (float4)(G[index + 0], G[index + 1], G[index + 2], 1.0f);
	return dot(v, g);
}
// Signed gradient noise 3d
float sgnoise3d(float4 position)
{

	float4 p = position;
	float4 pf = floor(p);
	int4 ip = (int4)((int)pf.x, (int)pf.y, (int)pf.z, 0.0);
	float4 fp = p - pf;        
    ip &= P_MASK;

    int4 I000 = (int4)(0, 0, 0, 0);
    int4 I001 = (int4)(0, 0, 1, 0);  
    int4 I010 = (int4)(0, 1, 0, 0);
    int4 I011 = (int4)(0, 1, 1, 0);
    int4 I100 = (int4)(1, 0, 0, 0);
    int4 I101 = (int4)(1, 0, 1, 0);
    int4 I110 = (int4)(1, 1, 0, 0);
    int4 I111 = (int4)(1, 1, 1, 0);
	
    float4 F000 = (float4)(0.0f, 0.0f, 0.0f, 0.0f);
    float4 F001 = (float4)(0.0f, 0.0f, 1.0f, 0.0f);
    float4 F010 = (float4)(0.0f, 1.0f, 0.0f, 0.0f);
    float4 F011 = (float4)(0.0f, 1.0f, 1.0f, 0.0f);
    float4 F100 = (float4)(1.0f, 0.0f, 0.0f, 0.0f);
    float4 F101 = (float4)(1.0f, 0.0f, 1.0f, 0.0f);
    float4 F110 = (float4)(1.0f, 1.0f, 0.0f, 0.0f);
    float4 F111 = (float4)(1.0f, 1.0f, 1.0f, 0.0f);
	
	float n000 = gradient3d(ip + I000, fp - F000);
	float n001 = gradient3d(ip + I001, fp - F001);
	
	float n010 = gradient3d(ip + I010, fp - F010);
	float n011 = gradient3d(ip + I011, fp - F011);
	
	float n100 = gradient3d(ip + I100, fp - F100);
	float n101 = gradient3d(ip + I101, fp - F101);

	float n110 = gradient3d(ip + I110, fp - F110);
	float n111 = gradient3d(ip + I111, fp - F111);

	float4 n40 = (float4)(n000, n001, n010, n011);
	float4 n41 = (float4)(n100, n101, n110, n111);

	float4 n4 = mix3d(n40, n41, smooth(fp.x));
	float2 n2 = mix2d(n4.xy, n4.zw, smooth(fp.y));
	float n = mix1d(n2.x, n2.y, smooth(fp.z));
	return n * (1.0f / 0.7f);
}
// Unsigned Gradient Noise 3d
float ugnoise3d(float4 position)
{
    return ( sgnoise3d(position));
}

uchar4 tonemap(float4 color)
{
    uchar4 result = convert_uchar4_sat_rte(color * 255.0f);
    return result;
}

float ugnoise3dOctaves(float4 position,int octaves,float persistence, float frequency, float amplitude){
	float total=0.0f;
	for(int i=0;i<octaves;i++){
		float4 sample=position*frequency;
		total+=ugnoise3d(sample)*amplitude;
		frequency*=2;
		amplitude*=persistence;
	}
	return total;
}

float4 sampleGradientNoiseVectorField(float4 position, int octaves,float persistence, float frequency, float amplitude){
float x=ugnoise3dOctaves(position,octaves,persistence,frequency,amplitude);
float4 offset=(float4){(float)1.1,(float)3.1,(float)1.1,(float)3.1};
float4 sample=vec4_add3(position,offset);
float y=ugnoise3dOctaves(sample,octaves,persistence,frequency,amplitude);
sample=vec4_add3(sample,offset);
float z=ugnoise3dOctaves(sample,octaves,persistence,frequency,amplitude);
return (float4)(x,y,z,0.0);
}

//======================= /3D Noise function =======================




//various structures 

//========================== Force Fields	=============================
//All the names of the forceFields are equivalent to maya force fields.

//newton field
typedef struct {
	float4 position; //attenuation is encoded in the 4. float value
	float magnitude;
	float maxDistance;
	float minDistance;
	float mass;
	//float attenuation; it's encoded in the pos.w 
} Fd_newton;

//radial field
typedef struct {
	float4 position; //attenuation is encoded in the 4. float value
	float magnitude;
	float maxDistance;
	//float attenuation; it's encoded in the pos.w 
} Fd_radial;

//uniform field
typedef struct {
	float4 position; //attenuation is encoded in the 4. float value
	float magnitude;
	float4 direction; //maxDistance is encoded in direction.w
	//float maxDistance;
} Fd_uniform;

//vortex field
typedef struct {
	float4 position; //attenuation is encoded in the 4. float value
	float magnitude;
	float4 axis; //maxDistance is encoded in axis.w
} Fd_vortex;

//drag field
typedef struct {
	float4 position; //attenuation is encoded in the 4. float value
	float4 direction;  //maxDistance is encoded in direction.w
	float magnitude;
	float speedAttenuation; //stupid naming, means minimal particle speed to affect the particle.
}Fd_drag;

//turbulence field
typedef struct {
	float4 position; //attenuation is encoded in the 4. float value
	float4 phase;  //maxDistance is encoded in phase.w
	float magnitude;
	float frequency; 
	float noiseRatio;
	int noiseLevel;
}Fd_turbulence;


//initializing force fields
//__constant Fd_uniform* unif= { ((float4)(0.0,0.3,0.1,0.0)), 10,((float4)(0.0,0.3,0.1,0.0)};
//========================== /Force Fields	=============================


//========================== Deflectors		=============================
//prozatim nevim jake parametry tam muzu cpat, nebo spise, ktere jsou vhodne
typedef struct {
	float4 position; //attenuation is encoded in the 4. float value
	float4 direction;  //maxDistance is encoded in direction.w
	float magnitude;
	float speedAttenuation; //stupid naming, means minimal particle speed to affect the particle.
}Def_sphere;


//========================== /Deflectors	=============================

////========================== Force calculations ===========================
//radial field
float4 fd_radial(float4 particlePosition,__constant Fd_radial* fd, int fdNum){
	float maxDistance=fd[fdNum].maxDistance;
	float4 position=fd[fdNum].position; //attenuation is encoded in the 4. float value
	
	//get rid of the encoded extrass
	particlePosition.w=0;
	position.w=0;
	float dist= distance(particlePosition, position);
	//won't affect particle if the particle is further than maxDistance
	if(maxDistance!=0&&maxDistance<dist){
		return (float4){0.0f, 0.0f, 0.0f, 0.0f};
	}
	
	float magnitude=fd->magnitude;
	float attenuation=position.w;
	
	//attenuation
	magnitude*=1.0/(pow((1+dist),attenuation));
	float4 force=normalize(vec4_sub3(particlePosition,position))*magnitude;
	return force;
	
}

//drag field
float4 fd_drag(float4 particlePosition,float4 particleVelocity,__constant Fd_drag* fd, int fdNum){
	float4 position=fd[fdNum].position; //attenuation is encoded in the 4. float value
	float4 direction=fd[fdNum].direction;  //maxDistance is encoded in direction.w
	float magnitude=fd[fdNum].magnitude;
	float speedAttenuation=fd[fdNum].speedAttenuation; //stupid naming, means minimal particle speed to affect the particle.
	
	float speed=length(particleVelocity);
	
	float attenuation=position.w;
	float maxDistance=direction.w;
	
	
	//get rid of the encoded extrass
	particlePosition.w=0;
	position.w=0;
	
	float dist= distance(particlePosition, position);
	
	if(speed<speedAttenuation || (maxDistance!=0 && maxDistance<dist)){
		return (float4){0.0f,0.0f,0.0f,0.0f};
	}
	
	
	magnitude*=-1.0/(pow((1+dist),attenuation));

	float4 force=particleVelocity*magnitude;
	
	return force;
}


/*
//working with global memory
//uniform field
float4 fd_uniform(float4 particlePosition, __global Fd_uniform* fd,int fdNum){
__global Fd_uniform* shit=&fd[fdNum];
	float4 position=shit->position; //attenuation is encoded in the 4. float value
	float magnitude=shit->magnitude;
	float4 direction=shit->direction; //maxDistance is encoded in direction.w
	
	float maxDistance=direction.w;
	float attenuation=position.w;
	
	//get rid of the encoded extrass
	particlePosition.w=0;
	position.w=0;
	
	float dist= distance(particlePosition, position);
	
	//won't affect particle if the particle is further than maxDistance
	if(maxDistance!=0&&maxDistance<dist){
		return (float4){0.0f, 0.0f, 0.0f, 0.0f};
	}
	
	//magnitude*=(-1.0/(pow((1+dist),attenuation)));
	float4 force=direction*magnitude;
	return force;
}
*/

//uniform field
float4 fd_uniform(float4 particlePosition, __constant Fd_uniform* fd,int fdNum){
	float4 position=fd[fdNum].position; //attenuation is encoded in the 4. float value
	float magnitude=fd[fdNum].magnitude;
	float4 direction=fd[fdNum].direction; //maxDistance is encoded in direction.w
	
	float maxDistance=direction.w;
	float attenuation=position.w;
	
	//get rid of the encoded extrass
	particlePosition.w=0;
	position.w=0;
	
	float dist= distance(particlePosition, position);
	
	//won't affect particle if the particle is further than maxDistance
	if(maxDistance!=0&&maxDistance<dist){
		return (float4){0.0f, 0.0f, 0.0f, 0.0f};
	}
	
	magnitude*=(-1.0/(pow((1+dist),attenuation)));
	float4 force=direction*magnitude;
	return force;
}

//vortex field
float4 fd_vortex(float4 particlePosition,__constant Fd_vortex* fd, int fdNum) {
	float4 position=fd[fdNum].position; //attenuation is encoded in the 4. float value
	float magnitude=fd[fdNum].magnitude;
	float4 axis=fd[fdNum].axis; //maxDistance is encoded in direction.w
	float attenuation=position.w;
	float maxDistance=axis.w;
	
	//get rid of the encoded extrass
	particlePosition.w=0;
	position.w=0;
	axis.w=0;
	
	float dist= distance(particlePosition, position);
	
	//won't affect particle if the particle is further than maxDistance
	if(maxDistance!=0&&maxDistance<dist){
		return (float4){0.0f, 0.0f, 0.0f, 0.0f};
	}
	
	//vactor from particle to point
	float4 vector=vec4_sub3(particlePosition,position); 
	
	//computes particle distance from axis
	//float particleDistance = length(cross(axis,vector)) / lengh(axis);
	
	magnitude*=1.0/(pow((1+dist),attenuation));
	float4 force=normalize(cross(vector,axis))*magnitude;
	
	return	force;
}

//newton field = particle attraction to some other object
float4 fd_newton(float4 particlePosition,float particleMass, __constant Fd_newton* fd,int fdNum) {
	float4 position=fd[fdNum].position; //attenuation is encoded in the 4. float value
	float magnitude=fd[fdNum].magnitude;
	float maxDistance=fd[fdNum].maxDistance;
	float minDistance=fd[fdNum].minDistance;
	float mass=fd[fdNum].mass;
	
	float attenuation=position.w;
	
	particlePosition.w=0;
	position.w=0;
	
	float dist= distance(particlePosition, position);
	if((maxDistance!=0&&maxDistance<dist)||(minDistance!=0&&minDistance>dist)){
		return (float4){0.0f, 0.0f, 0.0f, 0.0f};
	}
	magnitude*= 9.8*(particleMass*mass)/(dist*dist);
	float4 force=normalize(vec4_sub3(particlePosition,position))*magnitude;
	return force;
}



//turbulence field
float4 fd_turbulence(float4 particlePosition,__constant Fd_turbulence* fd, int fdNum){
	float4 position=fd[fdNum].position; //attenuation is encoded in the 4. float value
	float4 phase=fd[fdNum].phase;  //maxDistance is encoded in phase.w
	float magnitude=fd[fdNum].magnitude;
	float frequency=fd[fdNum].frequency; //stupid naming, means minimal particle speed to affect the particle.
	float noiseRatio=fd[fdNum].noiseRatio;
	int noiseLevel=fd[fdNum].noiseLevel;
	
	float attenuation=position.w;
	float maxDistance=phase.w;
	
	//get rid of the encoded extrass
	particlePosition.w=0;
	position.w=0;
	phase.w=0;
	
	float dist= distance(particlePosition, position);
	
	if(maxDistance!=0 && maxDistance<dist){
		return (float4){0.0f,0.0f,0.0f,0.0f};
	}
	
	magnitude*=1.0/(pow((1+dist),attenuation));
	float4 vc= normalize(sampleGradientNoiseVectorField(vec4_add3(particlePosition,phase), noiseLevel,noiseRatio,frequency, 10.0));
	//float4 koef=((float)sign(vc.x),(float)sign(vc.y),(float)sign(vc.z),0.0);
	
	
	float4 force=vc*magnitude;
	
	return force;
}

////========================== /Force calculations ===========================



// opencl contains functions for vector operations such as: lengh(),normalize(),dot(),cross(), distance()

__kernel void part2(
	__global float4* pos,			//0
	__global float4* color,			//1
	__global float4* vel,			//2
	__global float4* pos_gen,		//3
	__global float4* vel_gen,		//4
	float dt,						//5
	int pCount,						//6	
	__constant Fd_uniform* fd_uniforms,			//7
	__constant Fd_drag* fd_drags,				//8
	__constant Fd_radial* fd_radials,			//9
	__constant Fd_vortex* fd_vortices,			//10
	__constant Fd_turbulence* fd_turbulences,	//11
	__constant Fd_newton* fd_newtons			//12
)
{

    //get our index in the array
    unsigned int i = get_global_id(0);
	if(i>=pCount){
	return;
	}
    float4 p = pos[i];
	float4 v = vel[i];

    //we've stored the life in the fourth component of our velocity array
    float life = v.w;

    //decrease the life by the time step (this value could be adjusted to lengthen or shorten particle life
    life -= dt;
	if(life<=0.1){
		p=pos_gen[i];
		v=vel_gen[i];
		life=v.w;
	}
	
	//__global Fd_uniform* shit=fd_uniforms;
	
	//structy musej bejt nahrany v globalni pameti, opencl vubec netusi jak s nimi pracovat, neumi je vytvaret
	float4 fc=fd_uniform(p,fd_uniforms,0);
	//fc=vec4_add3(fc,fd_radial(p,fd_radials,0));
	//fc=vec4_add3(fc,fd_radial(p,fd_radials,1));
	fc=vec4_add3(fc,fd_vortex(p,fd_vortices,0));
	fc=vec4_add3(fc,fd_turbulence(p,fd_turbulences,0));
	//fc=vec4_add3(fc,fd_drag(p,v,fd_drags,0));

	
	v=vec4_add3(v,fc);
	p=vec4_add3(p,v*dt);

    //store the updated life in the velocity array
    v.w = life;

    //update the arrays with our newly computed values
    pos[i] = p;
    vel[i] = v;

    //you can manipulate the color based on properties of the system
    //here we adjust the alpha
	v=normalize(v);
    color[i] = vec4_add3(v,(float4)(0.5,0.5,0.5,0.5));
	color[i].w=life;
}



