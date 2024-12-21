/*
 * Skeleton code for COSE436 Fall 2024 Assignment 1
 *
 * Won-Ki Jeong, wkjeong@korea.ac.kr
 */

#include <stdio.h>
#include <GL/glew.h>
#include <GL/glut.h>

#include <iostream>
#include <assert.h>
#include "textfile.h"
#include "Angel.h"


// Shader programs
GLuint p;

int index = 0;
bool ortho = true;


void changeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if(h == 0) h = 1;

	float ratio = 1.0f * (float) w / (float)h;

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);
		
}

bool trans = false;
bool rotat = false;
bool keyX = false;
bool keyY = false;
bool keyZ = false;
bool keyX2 = false;
bool keyY2 = false;
bool keyZ2 = false;
float translateX = 0.0f;
float translateY = 0.0f;
float translateZ = 0.0f;
float rotateX = 0.0f;
float rotateY = 0.0f;
float rotateZ = 0.0f;

void keyboard(unsigned char key, int x, int y)
{
	// ToDo
	if (key == 'm') {
		index = (index + 1) % 3;
	}
	else if (key == 'o') {
		ortho = true;
	}
	else if (key == 'p') {
		ortho = false;
	}
	else if (key == 't') {
		trans = !trans;
	}
	else if (trans) {
		switch (key) {
		case 'x':
			keyX = true;
			break;
		case 'y':
			keyY = true;
			break;
		case 'z':
			keyZ = true;
			break;
		}
		trans = false;
	}
	else if (key == 'r') {
		rotat = !rotat;
	}
	else if (rotat) {
		switch (key) {
		case 'x':
			keyX2 = true;
			break;
		case 'y':
			keyY2 = true;
			break;
		case 'z':
			keyZ2 = true;
			break;
		}
		rotat = false;
	}
	
	glutPostRedisplay();
}

void specialKeys(int key, int x, int y)
{
	if (keyX) {
		switch (key) {
		case GLUT_KEY_RIGHT:
			translateX += 1.0f;
			break;
		case GLUT_KEY_LEFT:
			translateX -= 1.0f;
			break;
		}
		keyX = false;
	}
	else if (keyY) {
		switch (key) {
		case GLUT_KEY_RIGHT:
			translateY += 1.0f;
			break;
		case GLUT_KEY_LEFT:
			translateY -= 1.0f;
			break;
		}
		keyY = false;
	}
	else if (keyZ) {
		switch (key) {
		case GLUT_KEY_RIGHT:
			translateZ += 1.0f;
			break;
		case GLUT_KEY_LEFT:
			translateZ -= 1.0f;
			break;
		}
		keyZ = false;
	}
	else if (keyX2) {
		switch (key) {
		case GLUT_KEY_RIGHT:
			rotateX += 10.0f;
			break;
		case GLUT_KEY_LEFT:
			rotateX -= 10.0f;
			break;
		}
		keyX2 = false;
	}
	else if (keyY2) {
		switch (key) {
		case GLUT_KEY_RIGHT:
			rotateY += 10.0f;
			break;
		case GLUT_KEY_LEFT:
			rotateY -= 10.0f;
			break;
		}
		keyY2 = false;
	}
	else if (keyZ2) {
		switch (key) {
		case GLUT_KEY_RIGHT:
			rotateZ +=10.0f;
			break;
		case GLUT_KEY_LEFT:
			rotateZ -= 10.0f;
			break;
		}
		keyZ2 = false;
	}
	glutPostRedisplay();
}

void drawModel() {
	switch (index) {
	case 0:
		glColor3f(0.0, 1.0, 1.0);
		glutWireTeapot(1);
		break;
	case 1:
		glColor3f(1.0, 0.0, 0.1);
		glutWireSphere(1 , 16, 16);
		break;
	case 2:
		glColor3f(1.0, 1.0, 0.0);
		glutWireCube(1);
		break;	
	}
}

void Frame() {

	mat4 modelViewMatrix = mat4(1.0f);
	GLuint modelMatrixLoc = glGetUniformLocation(p, "model");
	glUniformMatrix4fv(modelMatrixLoc, 1, GL_TRUE, modelViewMatrix);

    glBegin(GL_LINES);
    
    // X
    glColor3f(1, 0, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(10, 0, 0);
    
    // Y
	glColor3f(0, 0, 1);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 10, 0);
    
    // Z
	glColor3f(0, 1, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, 10);
    
    glEnd();
}

void display() {

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	vec4 eye;

	// 투영 행렬 설정
	mat4 projMatrix;
	if (ortho) {
		projMatrix = Ortho(-2.0f, 2.0f, -2.0f, 2.0f, 0.1f, 100.0f);	  // Ortho(left, right, bottom, top, zNear, zFar)
		eye = vec4(50.0, 30.0, 50.0, 1.0);
	}
	else {
		projMatrix = Perspective(45.0f, 1.0f, 0.1f, 100.0f);   //Perspective(fovy, aspect, zNear, zFar)
		eye = vec4(5.0, 3.0, 5.0, 1.0);
	}

	vec4 at(0.0, 0.0, 0.0, 1.0);
	vec4 up(0.0, 1.0, 0.0, 0.0);
	mat4 viewMatrix = LookAt(eye, at, up);

	// model matrix
	mat4 modelViewMatrix = mat4(1.0f); // plain matrix
	mat4 moveToOrigin = Translate(-translateX, -translateY, -translateZ); // inverse matrix

	// rotation matirx
	mat4 rotateXMat = RotateX(rotateX);
	mat4 rotateYMat = RotateY(rotateY);
	mat4 rotateZMat = RotateZ(rotateZ);

	// translation
	mat4 moveBack = Translate(translateX, translateY, translateZ);

	// rotation
	modelViewMatrix = moveBack * rotateXMat * rotateYMat * rotateZMat;
	modelViewMatrix = modelViewMatrix * moveToOrigin * moveBack;

	GLuint modelMatrixLoc = glGetUniformLocation(p, "model");
	glUniformMatrix4fv(modelMatrixLoc, 1, GL_TRUE, modelViewMatrix);

	//// view matrix
	GLuint viewLoc = glGetUniformLocation(p, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_TRUE, viewMatrix);

	//projection matrix
	GLuint projLoc = glGetUniformLocation(p, "projection");
	glUniformMatrix4fv(projLoc, 1, GL_TRUE, projMatrix);

}

void renderScene(void) 
{
	// ToDo

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(p);
	display();

	drawModel();

	Frame();

	glutSwapBuffers();

}

void idle()
{
	glutPostRedisplay();
}


void init()
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.0, 1.0); 
	
	// Create shader program
	p = createGLSLProgram("C:/Users/User/Desktop/programing/Interactive_Visual/IV_assign_1/vshader.vert", NULL, "C:/Users/User/Desktop/programing/Interactive_Visual/IV_assign_1/fshader.frag");

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.0, 1.0);

}


int main(int argc, char **argv) {

	// init GLUT and create Window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(600,600);
	glutCreateWindow("COSE436 - Assignment 1");

	// register callbacks
	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialKeys);
	glutIdleFunc(idle);

	glewInit();
	if (glewIsSupported("GL_VERSION_3_3"))
		printf("Ready for OpenGL 3.3\n");
	else {
		printf("OpenGL 3.3 is not supported\n");
		exit(1);
	}

	init();
	

	// enter GLUT event processing cycle
	glutMainLoop();

	return 1;
}
