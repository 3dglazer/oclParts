#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <iomanip>
#include <math.h>
using	namespace std;

#if defined __APPLE__ || defined(MACOSX)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include "bislogon.h"
#include "OpenCLObject.h"


#define NUM_PARTICLES 1000000
OpenCLObject* oclProgram;

bool stop=true;
int window_width = 1280;
int window_height = 800;
int glutWindowHandle = 0;
float translate_z = -1.f;

int mouse_old_x, mouse_old_y;
int mouse_buttons = 0;
float rotate_x = 0.0, rotate_y = 0.0;

void init_gl(int argc, char** argv);
void appRender();
void appDestroy();
void timerFunc(int ms);
void appKeyboard(unsigned char key, int x, int y);
void appMouse(int button, int state, int x, int y);
void appMotion(int x, int y);

std::vector<Vec4> pos;
std::vector<Vec4> vel;
std::vector<Vec4> color;

float rand_float(float mn, float mx)
{
    float r = random() / (float) RAND_MAX;
    return mn + (mx-mn)*r;
}

int rand_int(int mn,int mx){
	return round(rand_float(mn, mx));
}


//generates random permutation 
void generatePermutation(int num){
	int rest=num-1;
	int genPole[num];
	for (int i=0; i<num; i++) {
		genPole[i]=-1;
	}
	int final[num];
	int count=0;
	printf("\n");
	while (rest!=0) {
		float test=rand_float(0.0,1.0);
		//printf("=== %f===",test);
		if (test<0.1) {
			if (genPole[count%num]==-1) {
				genPole[count%num]=count%num;
				final[rest]=count%num;
				rest--;
			};
			
		}
		count++;
	}
	for (int i=0; i<num; i++) {
		printf("%i, ",final[i]);
	}
printf("\n");
}

//Inits particle positions, velocities and colors randombly in the sphere
void initBuffers(std::vector<Vec4> &pos,std::vector<Vec4> &vel,std::vector<Vec4> &color, int numParticles){
    for(int i = 0; i < numParticles; i++)
    {
		float r= rand_float(.0, 0.5);
        float theta = rand_float(.0, 3.1415925535);
		float fi = rand_float(.0, 2.*3.1415925535);
		float x=r*sin(theta)*cos(fi);
		float y=r*sin(theta)*sin(fi);
		float z=r*cos(theta);
        pos[i] = Vec4(x, y, z, 1.0f);
        
        float life_r = rand_float(0.f, 6.f);
		life_r=6.0;
        vel[i] = Vec4(0.0, 0.0, 0.0f, life_r);
		
		float rndx=rand_float(0.0, 1.0);
		float rndy=rand_float(0.0, 1.0);
		float rndz=rand_float(0.0, 1.0);
        color[i] = Vec4(rndx, rndy,rndz, 1.0f);
    }
}

// Inits particle positions, velocities and colors according to the colorArray
void initBISBuffers(std::vector<Vec4> &pos,std::vector<Vec4> &vel,std::vector<Vec4> &color, int numParticles,int width,int height,unsigned char *colorArray){
	float colSC=1./255.;
	int sz=width*height-1;
	float scX=1./float(height);
	float scY=1./float(width);
	float offsetX=(width/2)*scX;
	float offsetY=-(height/2)*scY;
	for(int i = 0; i < numParticles; i++)
    {
		int coord=rand_int(0, sz);
		float coordX=(float)(coord%width)*scX-offsetX;
		float coordY=(float)(coord/height)*-scY-offsetY;
		int colorArrayCoord=coord*3;
		
		float r=colorArray[colorArrayCoord]*colSC;
		float g=colorArray[colorArrayCoord+1]*colSC;
		float b=colorArray[colorArrayCoord+2]*colSC;
		
        pos[i] = Vec4(coordX,coordY, 0., 1.0f);
        
        float life_r = rand_float(0.f, 6.f);
		life_r=.5;
        vel[i] = Vec4(0.0, 0.0, 0.0f, life_r);
        color[i] = Vec4(r,g,b, 1.0f);
    }
}

void initBISequal(std::vector<Vec4> &pos,std::vector<Vec4> &vel,std::vector<Vec4> &color, int numParticles,int width,int height,unsigned char *colorArray){
	float scX=1./float(width);
	float scY=1./float(height);
	float offsetX=((float)height/2.)*scX;
	float offsetY=-((float)width/2.)*scY;
	float colSC=1./255.;
	int i=0;
	for (int x=0; x<height; x++) {
		for (int y=0; y<width; y++) {
			float coordX=(float)(x)*scX-offsetX;
			float coordY=(float)(y)*scY-offsetY;
			int colorArrayCoord=(x*width+y)*3;
			
			float r=colorArray[colorArrayCoord]*colSC;
			float g=colorArray[colorArrayCoord+1]*colSC;
			float b=colorArray[colorArrayCoord+2]*colSC;
			
			pos[i] = Vec4(coordX,coordY, 0., 1.0f);
			
			float life_r = rand_float(0.f, 6.f);
			life_r=.5;
			vel[i] = Vec4(0.0, 0.0, 0.0f, life_r);
			color[i] = Vec4(r,g,b, 1.0f);
			i++;
		}
	}
}

int main(int argc, char** argv)
{
    printf("Hello, OpenCL\n");

    init_gl(argc, argv);
	printf("OPENGL HAS BEEN INITIALIZED");
    //initialize our CL object, this sets up the context
    oclProgram = new OpenCLObject();
	//load and build our CL program from the file
	printf("going to load the source n/");
    oclProgram->loadProgramSource("/myPrograms/GPUParticleSystem 2/particlesKernel.cl");
	
	printf("program has been loaded");
    int num = NUM_PARTICLES;
	//num=800*488;
    pos.resize(num);
	vel.resize(num);
    color.resize(num);
	
	initBISBuffers(pos,vel,color, num,800,488,colors);
	//initBISequal(pos,vel,color, num,800,488,colors);
	//initBuffers(pos,vel,color,num);
    
	printf("Vectors Have been initialized \n");
	//send data to GPU
    oclProgram->loadData(pos, vel, color);
    //initialize the kernel with created buffers
    oclProgram->initKernel();
    
    glutMainLoop();
	return 0;
}

void appRender()
{
	glClearColor(0.5,0.5,0.5,0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (!stop) {
		// Updating particle dynamics
		oclProgram->runKernel();
	}

    glEnable(GL_BLEND);
	
	//this is purely additive
    //glBlendFunc(GL_SRC_ALPHA,GL_ONE );
	//this works normaly
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_POINT_SMOOTH);
    glPointSize(1.);
    
    glBindBuffer(GL_ARRAY_BUFFER, oclProgram->color_vbo);
    glColorPointer(4, GL_FLOAT, 0, 0);
	
    glBindBuffer(GL_ARRAY_BUFFER, oclProgram->position_vbo);
    glVertexPointer(4, GL_FLOAT, 0, 0);
	
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDrawArrays(GL_POINTS, 0, oclProgram->num);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glutSwapBuffers();
}


void init_gl(int argc, char** argv)
{
	
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(window_width, window_height);
    glutInitWindowPosition (glutGet(GLUT_SCREEN_WIDTH)/2 - window_width/2,
                            glutGet(GLUT_SCREEN_HEIGHT)/2 - window_height/2);
	
    
    std::stringstream ss;
    ss << "Particle system running: " << NUM_PARTICLES << " particles" << std::ends;
    glutWindowHandle = glutCreateWindow(ss.str().c_str());
	
    glutDisplayFunc(appRender); 
    glutTimerFunc(0, timerFunc, 0); 
    glutKeyboardFunc(appKeyboard);
    glutMouseFunc(appMouse);
    glutMotionFunc(appMotion);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glDisable(GL_DEPTH_TEST);
    glViewport(0, 0, window_width, window_height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90.0, (GLfloat)window_width / (GLfloat) window_height, 0.1, 1000.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, translate_z);
	
}

void appDestroy()
{
    delete oclProgram;
    if(glutWindowHandle)glutDestroyWindow(glutWindowHandle);
    exit(0);
}

void timerFunc(int ms)
{
    glutTimerFunc(ms, timerFunc, ms);
    glutPostRedisplay();
}

void appKeyboard(unsigned char key, int x, int y)
{
    switch(key)
    {
        case '\033': 
			appDestroy();
			break;
		case 's':
			if (stop) {
				stop=false;
			}else {
				stop=true;
			}
			break;
    }
}

void appMouse(int button, int state, int x, int y)
{
    if (state == GLUT_DOWN) {
        mouse_buttons |= 1<<button;
    } else if (state == GLUT_UP) {
        mouse_buttons = 0;
    }
	
    mouse_old_x = x;
    mouse_old_y = y;
}

void appMotion(int x, int y)
{
    float dx, dy;
    dx = x - mouse_old_x;
    dy = y - mouse_old_y;
	
    if (mouse_buttons & 1) {
        rotate_x += dy * 0.2;
        rotate_y += dx * 0.2;
    } else if (mouse_buttons & 4) {
        translate_z += dy * 0.1;
    }
	
    mouse_old_x = x;
    mouse_old_y = y;
	
    // set view matrix
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, translate_z);
    glRotatef(rotate_x, 1.0, 0.0, 0.0);
    glRotatef(rotate_y, 0.0, 1.0, 0.0);
}