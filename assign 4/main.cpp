/*
 * Skeleton code for COSE436 Fall 2024
 *
 * Won-Ki Jeong, wkjeong@korea.ac.kr
 *
 */

#include <stdio.h>
#include <GL/glew.h>
#include <GL/glut.h>

#include <iostream>
#include <assert.h>
#include "textfile.h"
#include "tfeditor.h"

#include "Angel.h"

/*
#define FILE_NAME "../data/CThead_512_512_452.raw"
#define W 512
#define H 512
#define D 452
*/


#define FILE_NAME "../data/tooth_100_90_160.raw"
#define W 100
#define H 90
#define D 160


/*
#define FILE_NAME "../data/bonsai_256_256_256.raw"
#define W 256
#define H 256
#define D 256
*/

/*
#define FILE_NAME "../data/Bucky_32_32_32.raw"
#define W 32
#define H 32
#define D 32
*/

/*
#define FILE_NAME "../data/lung_256_256_128.raw"
#define W 256
#define H 256
#define D 128
*/


// Glut windows
int volumeRenderingWindow;
int transferFunctionWindow;

// Shader programs
GLuint p;

// Texture object
GLuint objectTex;

// Transfer value
GLuint trans1D;

// Projection Mode Mux
int mode = 0;
GLuint modeLoc;

// Isosurface value
float iso = 0.5;
float change = 0.02;
GLuint isoLoc;

// Virtual trackball instances
mat4 rotationMatrix = mat4(1.0f);
float zoom = 1.0f;

bool isRotating = false, isZooming = false;
int winWidth, winHeight;
float angle = 0.0f;
float axis[3] = { 0.0f, 0.0f, 0.0f };;
float lastPos[3] = { 0.0f, 0.0f, 0.0f };

void trackball_ptov(int x, int y, int width, int height, float v[3])
{
	float d, a;

	v[0] = (2.0F * x - width) / width;
	v[1] = (height - 2.0F * y) / height;
	d = (float)sqrt(v[0] * v[0] + v[1] * v[1]);
	v[2] = (float)cos((M_PI / 2.0F) * ((d < 1.0F) ? d : 1.0F));
	a = 1.0F / (float)sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	v[0] *= a;
	v[1] *= a;
	v[2] *= a;
}

void mouseClick(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		trackball_ptov(x, y, winWidth, winHeight, lastPos);
		isRotating = true;
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		isRotating = false;
	}

	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		trackball_ptov(x, y, winWidth, winHeight, lastPos);
		isZooming = true;
	}
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP) {
		isZooming = false;
	}
}

mat4 Rotate(float angle, float x, float y, float z) {

	float angleYZ = atan2(y, z) * 180.0f / M_PI;
	float angleXZ = atan2(x, sqrt(y * y + z * z)) * 180.0f / M_PI;

	mat4 rotation = RotateX(-angleYZ) * RotateY(-angleXZ) * RotateZ(angle) * RotateY(angleXZ) * RotateX(angleYZ);

	return rotation;
}

// Mouse motion callback for rotation, translation, and zoom transformations
void mouseMove(int x, int y) {

	float curPos[3], dx, dy, dz;
	trackball_ptov(x, y, winWidth, winHeight, curPos);

	dx = curPos[0] - lastPos[0];
	dy = curPos[1] - lastPos[1];
	dz = curPos[2] - lastPos[2];

	/* compute theta and cross product */
	angle = 90.0 * sqrt(dx * dx + dy * dy + dz * dz);
	axis[0] = lastPos[1] * curPos[2] - lastPos[2] * curPos[1];
	axis[1] = lastPos[2] * curPos[0] - lastPos[0] * curPos[2];
	axis[2] = lastPos[0] * curPos[1] - lastPos[1] * curPos[0];

	float length = sqrt(axis[0] * axis[0] + axis[1] * axis[1] + axis[2] * axis[2]);
	if (length > 0.0f) {
		axis[0] /= length;
		axis[1] /= length;
		axis[2] /= length;
	}

	if (isRotating && (dx || dy || dz))
	{
		mat4 rot = Rotate(angle, axis[0], axis[1], axis[2]);
		rotationMatrix = rot * rotationMatrix;
	}

	if (isZooming) {
		zoom += dx * 2.0f;
		if (zoom < 0.1f) zoom = 0.1f;
	}

	lastPos[0] = curPos[0];
	lastPos[1] = curPos[1];
	lastPos[2] = curPos[2];

	glutPostRedisplay();
}

//
// Loading volume file, create 3D texture and its histogram
//
void load3Dfile(char *filename,int w,int h,int d) {

	// loading volume data
	FILE *f = fopen(filename, "rb");
	unsigned char *data = new unsigned char[w*h*d];
	fread(data, 1, w*h*d, f);
	fclose(f);

	// generate 3D texture
	glGenTextures(1, &objectTex);
	glBindTexture(GL_TEXTURE_3D, objectTex);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, w, h, d, 0, GL_RED, GL_UNSIGNED_BYTE, data);


	// create histogram
	for (int i = 0; i<256; i++) {
		histogram[i] = 0;
	}
	for (int i = 0; i < w*h*d; i++) {
		histogram[data[i]]++;
	}
	for (int i = 0; i<256; i++) {
		histogram[i] /= w*h*d;
	}

	delete[]data;
}


void changeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if(h == 0) h = 1;
	float ratio = 1.0f * (float) w / (float)h;

	winWidth = w;
	winHeight = h;

	// Set the viewport to be the entire window
    glViewport(0, 0, w, h);
}


void keyboard(unsigned char key, int x, int y)
{
	if(key == 'p') {
		rotationMatrix = mat4(1.0f);
		zoom = 1.0f;
	}
	if (key == 'q') {
		exit(0);
	}
	if (key == '1') {
		//Maximum intensity
		mode = 1;
		glUniform1i(modeLoc, mode);
	}
	if (key == '2') {
		//Isosurface
		mode = 2;
		glUniform1i(modeLoc, mode);
	}
	if (key == '3') {
		//alpha composing
		mode = 0;
		glUniform1i(modeLoc, mode);
	}
	if (key == '4') {
		iso -= change;
		glUniform1f(isoLoc, iso);
		printf("iso value : %f\n", iso);
	}
	if (key == '5') {
		iso += change;
		glUniform1f(isoLoc, iso);
		printf("iso value : %f\n", iso);
	}

	glutPostRedisplay();
}

void display() {

	glLoadIdentity();

	vec3 eye(0.0, 0.0, -5.0);
	vec4 at(0.0, 0.0, 0.0, 1.0);
	vec3 up(0.0, 1.0, 0.0);
	mat4 viewMatrix = LookAt(vec4(eye, 1.0), at, vec4(up, 0.0));

	mat4 projMatrix = Perspective(45.0f, 1.0f , 0.1f, 100.0f);

	mat4 modelMatrix = rotationMatrix * Scale(zoom) * mat4(1.0f);

	GLuint modelMatrixLoc = glGetUniformLocation(p, "model");
	glUniformMatrix4fv(modelMatrixLoc, 1, GL_TRUE, modelMatrix);

	GLuint viewMatrixLoc = glGetUniformLocation(p, "view");
	glUniformMatrix4fv(viewMatrixLoc, 1, GL_TRUE, viewMatrix);

	GLuint projLoc = glGetUniformLocation(p, "proj");
	glUniformMatrix4fv(projLoc, 1, GL_TRUE, projMatrix);

	vec4 newEye = (transpose(rotationMatrix) * vec4(eye, 1.0));
	vec4 newUp = (transpose(rotationMatrix) * vec4(up, 0.0));

	GLuint eyeLoc = glGetUniformLocation(p, "eyePosition");
	glUniform3fv(eyeLoc, 1, &newEye[0]);

	GLuint upLoc = glGetUniformLocation(p, "up");
	glUniform3fv(upLoc, 1, &newUp[0]);
	
}

void texturing() {

	GLuint texSamplerLoc = glGetUniformLocation(p, "tex");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, objectTex);
	glUniform1i(texSamplerLoc, 0);

	vec3 objectMin = vec3(-0.5f, -0.5f, -0.5f);
	vec3 objectMax = vec3(0.5f, 0.5f, 0.5f);
	GLuint objectMinLoc = glGetUniformLocation(p, "objectMin");
	GLuint objectMaxLoc = glGetUniformLocation(p, "objectMax");
	glUniform3fv(objectMinLoc, 1, &objectMin[0]);
	glUniform3fv(objectMaxLoc, 1, &objectMax[0]);

	GLint uniformLoc = glGetUniformLocation(p, "transferFunction");
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_1D, trans1D);
	glTexSubImage1D(GL_TEXTURE_1D, 0, 0, 256, GL_RGBA, GL_FLOAT, transferFunction);
	glUniform1i(uniformLoc, 1);

	modeLoc = glGetUniformLocation(p, "mode");
	glUniform1i(modeLoc, mode);

	isoLoc = glGetUniformLocation(p, "isovalue");
	glUniform1f(isoLoc, iso);
}


void renderScene(void) 
{
	glClearColor(0, 0, 0, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(p);

	display();
	texturing();
	glutSolidCube(1.0f);
	

	// Debug transfer function values
	std::cout << "R,G,B,A: " << transferFunction[0] << "," << transferFunction[1] << "," << transferFunction[2] << "," << transferFunction[3] << std::endl;

	glutSwapBuffers();
}


void idle()
{
	if (transferFunctionChanged) {
		glutSetWindow(volumeRenderingWindow);
		transferFunctionChanged = false;
		glutPostRedisplay();
	}
}


void init() 
{
	load3Dfile(FILE_NAME, W, H, D);
	glUseProgram(p);
		
	for (int i = 0; i < 256; i++) {
		transferFunction[i * 4 + 0] = float(i) / 255.0;
		transferFunction[i * 4 + 1] = float(i) / 255.0;
		transferFunction[i * 4 + 2] = float(i) / 255.0;
		transferFunction[i * 4 + 3] = float(i) / 255.0;
	}

	glGenTextures(1, &trans1D);
	glBindTexture(GL_TEXTURE_1D, trans1D);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, 256, 0, GL_RGBA, GL_FLOAT, transferFunction);
	
}


int main(int argc, char **argv) 
{
	glutInit(&argc, argv);

	//
	// 1. Transfer Function Editor Window
	//
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 700);
	glutInitWindowSize(600, 300);
	transferFunctionWindow = glutCreateWindow("Transfer Function");

	// register callbacks
	glutDisplayFunc(renderScene_transferFunction);
	glutReshapeFunc(changeSize_transferFunction);

	glutMouseFunc(mouseClick_transferFunction);
	glutMotionFunc(mouseMove_transferFunction);
	glutIdleFunc(idle);

	init_transferFunction();

	//
	// 2. Main Volume Rendering Window
	//
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(600,600);
	volumeRenderingWindow = glutCreateWindow("Volume Rendering");

	// register callbacks
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutKeyboardFunc(keyboard);

	glutMouseFunc(mouseClick);
	glutMotionFunc(mouseMove);

	glutIdleFunc(idle);

	glEnable(GL_DEPTH_TEST);

	glewInit();
	if (glewIsSupported("GL_VERSION_3_3"))
		printf("Ready for OpenGL 3.3\n");
	else {
		printf("OpenGL 3.3 is not supported\n");
		exit(1);
	}

	// Create shader program
	p = createGLSLProgram( "../volumeRendering.vert", NULL, "../volumeRendering.frag" );

	init();

	// enter GLUT event processing cycle
	glutMainLoop();

	return 1;
}