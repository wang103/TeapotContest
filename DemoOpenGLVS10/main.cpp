//  
//	CS 418, Spring 2011
//	MP3 - Teapot Contest
//	By Tianyi Wang
//

#include <stdlib.h>
#include "glew.h"
#include <GL/glut.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "mesh_data.h"
#include "general_utils.h"

int isRotating = 1;			// 1 means rotate, 0 means do not rotate
const int nFPS = 60;
float fRotateAngle = 0.f;

int env_texture_on = 1;		// whether or not environment texture is applied
int teapot_texture_on = 1;	// whether or not teapot texture is applied
int lighting_on = 1;		// whether or not lighting is on

MeshData *teapotMeshData;

void loadTeapotMesh()
{
	teapotMeshData = new MeshData("teapot.obj");
}

/**
 *	Initialize the lighting in the scene.
 */
void initLighting(void)
{
	GLfloat white[] = {1.0, 1.0, 1.0, 0.0};
	GLfloat lpos[] = {2.0, 2.0, 2.0, 0.0};

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT0, GL_POSITION, lpos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, white);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white);
}

GLuint env_texture;
GLuint teapot_texture;

/**
 *	Initialize regular texture.
 */
void initTeapotTexture(void)
{
	// allocate a texture name
	glGenTextures(1, &teapot_texture);
	
	glActiveTextureARB(GL_TEXTURE1_ARB);

	// bind the texture
	glBindTexture(GL_TEXTURE_2D, teapot_texture);
	
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);

	// set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);

	// load the texture and bind it
	int result = BuildTexture("teapot-texture.jpg");
	if (result == 0)
	{
		perror("BuildTexture");
		getchar();
		exit(1);
	}
}

/**
 *	Initialize sphere mapping.
 */
void initSphereMapping(void)
{
	// allocate a texture name
	glGenTextures(1, &env_texture);

	glActiveTextureARB(GL_TEXTURE0_ARB);

	// bind the texture
	glBindTexture(GL_TEXTURE_2D, env_texture);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);

	// select modulate to mix texture with color for shading
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	
	// set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

	// load the texture and bind it
	int result = BuildTexture("env-texture.jpg");
	if (result == 0)
	{
		perror("BuildTexture");
		getchar();
		exit(1);
	}
}

void init(void)
{
	initLighting();

	initSphereMapping();
	initTeapotTexture();

	glClearColor(0.22, 0.69, 0.87, 0.0);	// set clear color to be sky blue
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);

	loadTeapotMesh();
}

void draw_teapot()
{
	teapotMeshData->draw();
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// reset OpenGL transformation matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// setup look at transformation
	gluLookAt(0.f,1.f,5.f,
		0.f,0.f,0.f,
		0.f,1.f,0.f);

	glTranslatef(0.f, -2.f, 0.f);
	glRotatef(fRotateAngle, 0.f, 1.f, 0.f);

	// draw teapot
	draw_teapot();

	glutSwapBuffers();	// swap front/back framebuffer to avoid flickering 
}

void reshape (int w, int h)
{
	// reset viewport ( drawing screen ) size
	glViewport(0, 0, w, h);

	float fAspect = ((float)w)/h; 

	// reset OpenGL projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70.f,fAspect,0.001f,30.f); 
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
	case 'q':
		// ESC or q hit, quit
		delete teapotMeshData;
		glDeleteTextures(1, &env_texture);
		glDeleteTextures(1, &teapot_texture);
		exit(0);
		break;

	case 'r':								// switch between rotate and not rotate
		isRotating = !isRotating;
		break;

	case 'c':								// switch between having and not having teapot texture
		teapot_texture_on = !teapot_texture_on;
		if (teapot_texture_on) {
			glActiveTextureARB(GL_TEXTURE1_ARB);
			glEnable(GL_TEXTURE_2D);
		}
		else {
			glActiveTextureARB(GL_TEXTURE1_ARB);
			glDisable(GL_TEXTURE_2D);
		}
		break;

	case 'v':								// switching between having or not having environment mapping
		env_texture_on = !env_texture_on;
		if (env_texture_on) {
			glActiveTextureARB(GL_TEXTURE0_ARB);
			glEnable(GL_TEXTURE_2D);
		}
		else {
			glActiveTextureARB(GL_TEXTURE0_ARB);
			glDisable(GL_TEXTURE_2D);
		}
		break;
		
	case 'b':								// switching between enabling and not enabling lighting
		lighting_on = !lighting_on;
		if (lighting_on) {
			glEnable(GL_LIGHTING);
		}
		else {
			glDisable(GL_LIGHTING);
		}
		break;
	}
}

void mouse(int button, int state, int x, int y)
{
	// not used in this MP
}

void timer(int v)
{
	if (isRotating == 1)
		fRotateAngle -= 1.f;

	glutPostRedisplay();				// trigger display function by sending redraw into message queue
	glutTimerFunc(1000/nFPS, timer, v);	// restart timer again
}

int main(int argc, char* argv[])
{
	glutInit(&argc, (char**)argv);

	// set up for double-buffering & RGB color buffer & depth test
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 500); 
	glutInitWindowPosition(100, 100);
	glutCreateWindow ((const char*)"Teapot Contest -- Tianyi Wang");

	glewInit();			// initialize glew library
	init();				// setting up user data & OpenGL environment
	
	// set up the call-back functions 
	glutDisplayFunc(display);			// called when drawing 
	glutReshapeFunc(reshape);			// called when change window size
	glutKeyboardFunc(keyboard);			// called when received keyboard interaction
	glutMouseFunc(mouse);				// called when received mouse interaction
	glutTimerFunc(100, timer, nFPS);	// a periodic timer. Usually used for updating animation
	
	glutMainLoop();		// start the main message-callback loop

	return 0;
}
