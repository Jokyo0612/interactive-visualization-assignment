/*
 * Skeleton code for COSE436 Fall 2024 Assignment 3
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
#include "Angel.h"

//
// Definitions
//

typedef struct{
    unsigned char x, y, z, w;
} uchar4;
typedef unsigned char uchar;

// BMP loader
void LoadBMPFile(uchar4 **dst, int *width, int *height, const char *name);

// Virtual trackball instances
mat4 rotationMatrix = mat4(1.0f);  // Rotation matrix
vec3 translation(0.0f, 0.0f, 0.0f);  // Translation vector
float zoom = 1.0f;

// Animation
mat4 afterChange = mat4(1.0f);
bool animeOn = true;

bool isRotating = false, isTranslating = false, isZooming = false;
int winWidth, winHeight;
float angle = 0.0f;
float axis[3] = { 0.0f, 0.0f, 0.0f };;
float lastPos[3] = { 0.0f, 0.0f, 0.0f };

// Shader programs
GLuint p, q;

// Texture
GLuint color_tex[10];

bool newReflec = true;
GLuint cubemapTexture;
GLuint framebuffer, depthBuffer;

// Buffer Object
GLuint VAO, VBO, TBO, IBO;

// Texture Coordinate
GLfloat slime[] = {
	0.33f, 0.25f, 0.66f, 0.25f, 0.66f, 0.5f, 0.33f, 0.5f,
	0.33f, 1.0f, 0.66f, 1.0f, 0.66f, 0.75f, 0.33f, 0.75f,
	0.0f, 0.75f, 0.0f, 0.5f, 0.33f, 0.5f, 0.33f, 0.75f,
	1.0f, 0.75f, 1.0f, 0.5f, 0.66f, 0.5f, 0.66f, 0.75f,
	0.33f, 0.75f, 0.66f, 0.75f, 0.66f, 0.5f, 0.33f, 0.5f,
	0.33f, 0.0f, 0.66f, 0.0f, 0.66f, 0.25f, 0.33f, 0.25f
};

GLfloat ground[] = {
	0.0f, 0.0f, 20.0f, 0.0f, 20.0f, 0.5f, 0.0f, 0.5f,
	0.0f, 0.0f, 20.0f, 0.0f, 20.0f, 0.5f, 0.0f, 0.5f,
	0.0f, 0.0f, 20.0f, 0.0f, 20.0f, 0.5f, 0.0f, 0.5f,
	0.0f, 0.0f, 20.0f, 0.0f, 20.0f, 0.5f, 0.0f, 0.5f,
	0.0f, 20.0f, 20.0f, 20.0f, 20.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 20.0f, 20.0f, 20.0f, 20.0f, 0.0f, 0.0f, 0.0f
};

GLfloat steve[] = {
	812.0f / 1474.0f, 590.0f / 1172.0f, 1114.0f / 1474.0f, 590.0f / 1172.0f,
	1114.0f / 1474.0f, 800.0f / 1172.0f, 812.0f / 1474.0f, 800.0f / 1172.0f,
	506.0f / 1474.0f, 590.0f / 1172.0f, 205.0f / 1474.0f, 590.0f / 1172.0f,
	205.0f / 1474.0f, 800.0f / 1172.0f, 506.0f / 1474.0f, 800.0f / 1172.0f,
	509.0f / 1474.0f, 590.0f / 1172.0f, 809.0f / 1474.0f, 590.0f / 1172.0f,
	809.0f / 1474.0f, 800.0f / 1172.0f, 509.0f / 1474.0f, 800.0f / 1172.0f,
	1416.0f / 1474.0f, 590.0f / 1172.0f, 1115.0f / 1474.0f, 590.0f / 1172.0f,
	1115.0f / 1474.0f, 800.0f / 1172.0f, 1416.0f / 1474.0f, 800.0f / 1172.0f,
	816.0f / 1474.0f, 800.0f / 1172.0f, 1118.0f / 1474.0f, 800.0f / 1172.0f,
	1118.0f / 1474.0f, 1100.0f / 1172.0f, 816.0f / 1474.0f, 1100.0f / 1172.0f,
	816.0f / 1474.0f, 586.0f / 1172.0f, 1118.0f / 1474.0f, 586.0f / 1172.0f,
	1118.0f / 1474.0f, 284.0f / 1172.0f, 816.0f / 1474.0f, 284.0f / 1172.0f,

	450.0f / 1474.0f, 245.0f / 1172.0f, 258.0f / 1474.0f, 245.0f / 1172.0f,
	258.0f / 1474.0f, 53.0f / 1172.0f, 450.0f / 1474.0f, 53.0f / 1172.0f,
	648.0f / 1474.0f, 245.0f / 1172.0f, 840.0f / 1474.0f, 245.0f / 1172.0f,
	840.0f / 1474.0f, 53.0f / 1172.0f, 648.0f / 1474.0f, 53.0f / 1172.0f,
	645.0f / 1474.0f, 245.0f / 1172.0f, 455.0f / 1474.0f, 245.0f / 1172.0f,
	455.0f / 1474.0f, 53.0f / 1172.0f, 645.0f / 1474.0f, 53.0f / 1172.0f,
	64.0f / 1474.0f, 245.0f / 1172.0f, 255.0f / 1474.0f, 245.0f / 1172.0f,
	255.0f / 1474.0f, 53.0f / 1172.0f, 64.0f / 1474.0f, 53.0f / 1172.0f,
	450.0f / 1474.0f, 250.0f / 1172.0f, 260.0f / 1474.0f, 250.0f / 1172.0f,
	260.0f / 1474.0f, 440.0f / 1172.0f, 450.0f / 1474.0f, 440.0f / 1172.0f,
	450.0f / 1474.0f, 250.0f / 1172.0f, 260.0f / 1474.0f, 250.0f / 1172.0f,
	260.0f / 1474.0f, 440.0f / 1172.0f, 450.0f / 1474.0f, 440.0f / 1172.0f,

	245.0f / 1474.0f, 920.0f / 1172.0f, 315.0f / 1474.0f, 920.0f / 1172.0f,
	315.0f / 1474.0f, 1058.0f / 1172.0f, 245.0f / 1474.0f, 1058.0f / 1172.0f,
	245.0f / 1474.0f, 920.0f / 1172.0f, 315.0f / 1474.0f, 920.0f / 1172.0f,
	315.0f / 1474.0f, 1058.0f / 1172.0f, 245.0f / 1474.0f, 1058.0f / 1172.0f,
	170.0f / 1474.0f, 920.0f / 1172.0f, 240.0f / 1474.0f, 920.0f / 1172.0f,
	240.0f / 1474.0f, 1058.0f / 1172.0f, 170.0f / 1474.0f, 1058.0f / 1172.0f,
	170.0f / 1474.0f, 920.0f / 1172.0f, 240.0f / 1474.0f, 920.0f / 1172.0f,
	240.0f / 1474.0f, 1058.0f / 1172.0f, 170.0f / 1474.0f, 1058.0f / 1172.0f,
	244.0f / 1474.0f, 1060.0f / 1172.0f, 313.0f / 1474.0f, 1060.0f / 1172.0f,
	313.0f / 1474.0f, 1131.0f / 1172.0f, 245.0f / 1474.0f, 1131.0f / 1172.0f,
	241.0f / 1474.0f, 845.0f / 1172.0f, 170.0f / 1474.0f, 845.0f / 1172.0f,
	170.0f / 1474.0f, 917.0f / 1172.0f, 241.0f / 1474.0f, 917.0f / 1172.0f,

	245.0f / 1474.0f, 920.0f / 1172.0f, 315.0f / 1474.0f, 920.0f / 1172.0f,
	315.0f / 1474.0f, 1058.0f / 1172.0f, 245.0f / 1474.0f, 1058.0f / 1172.0f,
	245.0f / 1474.0f, 920.0f / 1172.0f, 315.0f / 1474.0f, 920.0f / 1172.0f,
	315.0f / 1474.0f, 1058.0f / 1172.0f, 245.0f / 1474.0f, 1058.0f / 1172.0f,
	170.0f / 1474.0f, 920.0f / 1172.0f, 240.0f / 1474.0f, 920.0f / 1172.0f,
	240.0f / 1474.0f, 1058.0f / 1172.0f, 170.0f / 1474.0f, 1058.0f / 1172.0f,
	170.0f / 1474.0f, 920.0f / 1172.0f, 240.0f / 1474.0f, 920.0f / 1172.0f,
	240.0f / 1474.0f, 1058.0f / 1172.0f, 170.0f / 1474.0f, 1058.0f / 1172.0f,
	244.0f / 1474.0f, 1060.0f / 1172.0f, 313.0f / 1474.0f, 1060.0f / 1172.0f,
	313.0f / 1474.0f, 1131.0f / 1172.0f, 245.0f / 1474.0f, 1131.0f / 1172.0f,
	241.0f / 1474.0f, 845.0f / 1172.0f, 170.0f / 1474.0f, 845.0f / 1172.0f,
	170.0f / 1474.0f, 917.0f / 1172.0f, 241.0f / 1474.0f, 917.0f / 1172.0f,

	10.0f / 1474.0f, 595.0f / 1172.0f, 10.0f / 1474.0f, 527.0f / 1172.0f,
	80.0f / 1474.0f, 527.0f / 1172.0f, 80.0f / 1474.0f, 595.0f / 1172.0f,
	10.0f / 1474.0f, 745.0f / 1172.0f, 10.0f / 1474.0f, 815.0f / 1172.0f, 
	80.0f / 1474.0f, 815.0f / 1172.0f, 80.0f / 1474.0f, 745.0f / 1172.0f,
	10.0f / 1474.0f, 740.0f / 1172.0f, 10.0f / 1474.0f, 600.0f / 1172.0f, 
	110.0f / 1474.0f, 600.0f / 1172.0f, 110.0f / 1474.0f, 740.0f / 1172.0f,
	10.0f / 1474.0f, 740.0f / 1172.0f, 10.0f / 1474.0f, 600.0f / 1172.0f, 
	110.0f / 1474.0f, 600.0f / 1172.0f, 110.0f / 1474.0f, 740.0f / 1172.0f,
	0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,

	10.0f / 1474.0f, 595.0f / 1172.0f, 10.0f / 1474.0f, 527.0f / 1172.0f,
	80.0f / 1474.0f, 527.0f / 1172.0f, 80.0f / 1474.0f, 595.0f / 1172.0f,
	10.0f / 1474.0f, 745.0f / 1172.0f, 10.0f / 1474.0f, 815.0f / 1172.0f,
	80.0f / 1474.0f, 815.0f / 1172.0f, 80.0f / 1474.0f, 745.0f / 1172.0f,
	10.0f / 1474.0f, 740.0f / 1172.0f, 10.0f / 1474.0f, 600.0f / 1172.0f,
	110.0f / 1474.0f, 600.0f / 1172.0f, 110.0f / 1474.0f, 740.0f / 1172.0f,
	10.0f / 1474.0f, 740.0f / 1172.0f, 10.0f / 1474.0f, 600.0f / 1172.0f,
	110.0f / 1474.0f, 600.0f / 1172.0f, 110.0f / 1474.0f, 740.0f / 1172.0f,
	0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f
};

GLfloat cubeN[] = {
	// 앞면
	0.0f, 0.0f, -1.0f, // 왼쪽 아래 앞
	0.0f, 0.0f, -1.0f, // 오른쪽 아래 앞
	0.0f, 0.0f, -1.0f, // 오른쪽 위 앞
	0.0f, 0.0f, -1.0f, // 왼쪽 위 앞

	// 뒷면
	0.0f, 0.0f, 1.0f,  // 왼쪽 아래 뒤
	0.0f, 0.0f, 1.0f,  // 오른쪽 아래 뒤
	0.0f, 0.0f, 1.0f,  // 오른쪽 위 뒤
	0.0f, 0.0f, 1.0f,  // 왼쪽 위 뒤

	// 왼쪽 면
	-1.0f, 0.0f, 0.0f, // 왼쪽 아래 뒤
	-1.0f, 0.0f, 0.0f, // 왼쪽 아래 앞
	-1.0f, 0.0f, 0.0f, // 왼쪽 위 앞
	-1.0f, 0.0f, 0.0f, // 왼쪽 위 뒤

	// 오른쪽 면
	1.0f, 0.0f, 0.0f,  // 오른쪽 아래 뒤
	1.0f, 0.0f, 0.0f,  // 오른쪽 아래 앞
	1.0f, 0.0f, 0.0f,  // 오른쪽 위 앞
	1.0f, 0.0f, 0.0f,  // 오른쪽 위 뒤

	// 윗면
	0.0f, 1.0f, 0.0f,  // 왼쪽 위 앞
	0.0f, 1.0f, 0.0f,  // 오른쪽 위 앞
	0.0f, 1.0f, 0.0f,  // 오른쪽 위 뒤
	0.0f, 1.0f, 0.0f,  // 왼쪽 위 뒤

	// 아래면
	0.0f, -1.0f, 0.0f, // 왼쪽 아래 앞
	0.0f, -1.0f, 0.0f, // 오른쪽 아래 앞
	0.0f, -1.0f, 0.0f, // 오른쪽 아래 뒤
	0.0f, -1.0f, 0.0f  // 왼쪽 아래 뒤
};

// Gen Cube
#define MAX 1000
GLfloat vertices[MAX];
GLuint indices[MAX];
GLuint currentIndex = 0;
int vertexIndex = 0;
int indexIndex = 0;

void PolyCL() {
	vertexIndex = 0;
	indexIndex = 0;
	currentIndex = 0;
}

void addCube(float x, float y, float z, float width, float height, float depth, GLuint& currentIndex) {

	GLfloat cubeVertices[] = {
		// 앞면
		x - width / 2, y - height / 2, z - depth / 2, // 왼쪽 아래 앞
		x + width / 2, y - height / 2, z - depth / 2, // 오른쪽 아래 앞
		x + width / 2, y + height / 2, z - depth / 2, // 오른쪽 위 앞
		x - width / 2, y + height / 2, z - depth / 2, // 왼쪽 위 앞

		// 뒷면
		x - width / 2, y - height / 2, z + depth / 2, // 왼쪽 아래 뒤
		x + width / 2, y - height / 2, z + depth / 2, // 오른쪽 아래 뒤
		x + width / 2, y + height / 2, z + depth / 2, // 오른쪽 위 뒤
		x - width / 2, y + height / 2, z + depth / 2, // 왼쪽 위 뒤

		// 왼쪽 면
		x - width / 2, y - height / 2, z + depth / 2, // 왼쪽 아래 뒤
		x - width / 2, y - height / 2, z - depth / 2, // 왼쪽 아래 앞
		x - width / 2, y + height / 2, z - depth / 2, // 왼쪽 위 앞
		x - width / 2, y + height / 2, z + depth / 2, // 왼쪽 위 뒤

		// 오른쪽 면
		x + width / 2, y - height / 2, z + depth / 2, // 오른쪽 아래 뒤
		x + width / 2, y - height / 2, z - depth / 2, // 오른쪽 아래 앞
		x + width / 2, y + height / 2, z - depth / 2, // 오른쪽 위 앞
		x + width / 2, y + height / 2, z + depth / 2, // 오른쪽 위 뒤

		// 위면
		x - width / 2, y + height / 2, z - depth / 2, // 왼쪽 위 앞
		x + width / 2, y + height / 2, z - depth / 2, // 오른쪽 위 앞
		x + width / 2, y + height / 2, z + depth / 2, // 오른쪽 위 뒤
		x - width / 2, y + height / 2, z + depth / 2, // 왼쪽 위 뒤

		// 아래면
		x - width / 2, y - height / 2, z - depth / 2, // 왼쪽 아래 앞
		x + width / 2, y - height / 2, z - depth / 2, // 오른쪽 아래 앞
		x + width / 2, y - height / 2, z + depth / 2, // 오른쪽 아래 뒤
		x - width / 2, y - height / 2, z + depth / 2  // 왼쪽 아래 뒤
	};

	GLuint cubeIndices[] = {
		0, 1, 2, 0, 2, 3,       // 앞면
		4, 5, 6, 4, 6, 7,       // 뒷면
		8, 9, 10, 8, 10, 11,    // 왼쪽 면
		12, 13, 14, 12, 14, 15, // 오른쪽 면
		16, 17, 18, 16, 18, 19, // 위면
		20, 21, 22, 20, 22, 23  // 아래면
	};

	for (int i = 0; i < 24 * 3; i++) {
		vertices[vertexIndex++] = cubeVertices[i];
	}

	for (int i = 0; i < 6 * 6; i++) {
		indices[indexIndex++] = currentIndex + cubeIndices[i];
	}

	currentIndex += 24;
}

void head(float x, float y, float z, float scale) {
	addCube(x, y + scale * 0.6f, z, scale * 0.48f, scale * 0.48f, scale * 0.48f, currentIndex);
}

void body(float x, float y, float z, float scale) {
	addCube(x, y, z, scale * 0.5f, scale * 0.75f, scale * 0.25f, currentIndex);
}

void armLeft(float x, float y, float z, float scale) {
	addCube(x - scale * 0.375f, y, z, scale * 0.25f, scale * 0.75f, scale * 0.25f, currentIndex);
}

void armRight(float x, float y, float z, float scale) {
	addCube(x + scale * 0.375f, y, z, scale * 0.25f, scale * 0.75f, scale * 0.25f, currentIndex);
}

void legLeft(float x, float y, float z, float scale) {
	addCube(x - scale * 0.125f, y - scale * 0.75f, z, scale * 0.25f, scale * 0.75f, scale * 0.25f, currentIndex);
}

void legRight(float x, float y, float z, float scale) {
	addCube(x + scale * 0.125f, y - scale * 0.75f, z, scale * 0.25f, scale * 0.75f, scale * 0.25f, currentIndex);
}

void Person(float x, float y, float z, float scale) {
	head(x, y, z, scale);
	body(x, y, z, scale);
	armLeft(x, y, z, scale);
	armRight(x, y, z, scale);
	legLeft(x, y, z, scale);
	legRight(x, y, z, scale);
}

void buffer(GLfloat* vertices, GLfloat VS, GLuint* indices, GLfloat IS, GLfloat* texture, GLfloat TS) {

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, VS * vertexIndex / MAX, vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &TBO);
	glBindBuffer(GL_ARRAY_BUFFER, TBO);
	glBufferData(GL_ARRAY_BUFFER, TS, texture, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, IS * indexIndex / MAX, indices, GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void bufferR(GLfloat* vertices, GLfloat VS, GLuint* indices, GLfloat IS, GLfloat* texture, GLfloat TS) {

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, VS * vertexIndex / MAX, vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &TBO);
	glBindBuffer(GL_ARRAY_BUFFER, TBO);
	glBufferData(GL_ARRAY_BUFFER, TS, texture, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, IS * indexIndex / MAX, indices, GL_STATIC_DRAW);

	glBindVertexArray(0);
}

GLuint createCubeMap(int width, int height) {

	glGenTextures(1, &cubemapTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

	for (GLuint i = 0; i < 6; i++) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA,
			width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return cubemapTexture;
}

GLuint createFrameBuffer() {

	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return framebuffer;
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
	if (key == 'p') {
		rotationMatrix = mat4(1.0f);
		translation = vec3(0.0f, 0.0f, 0.0f);
		zoom = 1.0f;
	}
	if (key == 'q') {
		glDeleteTextures(10, color_tex);
		exit(0);
	}
	if (key == 'a') {
		animeOn = !animeOn;
	}
	glutPostRedisplay();
}

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

void mouseButton(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		trackball_ptov(x, y, winWidth, winHeight, lastPos);
		isRotating = true;
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		isRotating = false;
	}

	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		trackball_ptov(x, y, winWidth, winHeight, lastPos);
		isTranslating = true;
	}
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP) {
		isTranslating = false;
	}

	if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) {
		trackball_ptov(x, y, winWidth, winHeight, lastPos);
		isZooming = true;
	}
	else if (button == GLUT_MIDDLE_BUTTON && state == GLUT_UP) {
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
void mouseMotion(int x, int y) {
	
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

	if (isTranslating) {
		translation[0] -= dx * 10.0f;
		translation[1] += dy * 10.0f;
	}

	if (isZooming) {
		translation[2] -= dy * 15.0f;
		zoom += dx * 2.0f;
		if (zoom < 0.1f) zoom = 0.1f;
	}

	lastPos[0] = curPos[0];
	lastPos[1] = curPos[1];
	lastPos[2] = curPos[2];

	glutPostRedisplay();
}

void display(GLuint pi) {

	glLoadIdentity();
	
	vec4 eye(0.0, 10.0, -20.0, 1.0);
	vec4 at(0.0, 0.0, 0.0, 1.0);
	vec4 up(0.0, 1.0, 0.0, 0.0);
	mat4 viewMatrix = LookAt(eye, at, up);

	mat4 projMatrix = Perspective(45.0f, (float)winWidth / (float)winHeight, 0.1f, 1000.0f);

	mat4 modelMatrix = Translate(translation[0], translation[1], translation[2]) * rotationMatrix * Scale(zoom) * mat4(1.0f);
	
	GLuint modelMatrixLoc = glGetUniformLocation(pi, "model");
	glUniformMatrix4fv(modelMatrixLoc, 1, GL_TRUE, modelMatrix);

	GLuint viewMatrixLoc = glGetUniformLocation(pi, "view");
	glUniformMatrix4fv(viewMatrixLoc, 1, GL_TRUE, viewMatrix);

	GLuint projLoc = glGetUniformLocation(pi, "proj");
	glUniformMatrix4fv(projLoc, 1, GL_TRUE, projMatrix);

	glUniformMatrix4fv(glGetUniformLocation(p, "afterChange"), 1, GL_TRUE, afterChange);

	if (pi == q) {
		GLuint camLoc = glGetUniformLocation(pi, "cameraPos");
		glUniform3fv(camLoc, 1, vec3(eye.x, eye.y, eye.z));
	}
}

void drawPolygon(int i, GLint color_tex, GLint indSize) {
	glActiveTexture(GL_TEXTURE0 + i);
	
	GLuint texSamplerLoc;

	if (i == 5) {
		glBindTexture(GL_TEXTURE_CUBE_MAP, color_tex);
		texSamplerLoc = glGetUniformLocation(q, "texSampler");
		glUniform1i(texSamplerLoc, i);
	}
	else {
		glBindTexture(GL_TEXTURE_2D, color_tex);
		texSamplerLoc = glGetUniformLocation(p, "texSampler");
		glUniform1i(texSamplerLoc, i);
	}

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indSize * indexIndex / MAX, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void renderScene(void);

void updateCubeMap(GLuint cubemapTexture, vec3 position, GLuint framebuffer, GLuint width, GLuint height) {

	glUseProgram(p);

	mat4 captureViews[] = {
		LookAt(position, position + vec3(1.0f,  0.0f,  0.0f), vec3(0.0f, -1.0f,  0.0f)), // +X
		LookAt(position, position + vec3(-1.0f,  0.0f,  0.0f), vec3(0.0f, -1.0f,  0.0f)), // -X
		LookAt(position, position + vec3(0.0f,  1.0f,  0.0f), vec3(0.0f,  0.0f, -1.0f)), // +Y
		LookAt(position, position + vec3(0.0f, -1.0f,  0.0f), vec3(0.0f,  0.0f,  1.0f)), // -Y
		LookAt(position, position + vec3(0.0f,  0.0f,  1.0f), vec3(0.0f, -1.0f,  0.0f)), // +Z
		LookAt(position, position + vec3(0.0f,  0.0f, -1.0f), vec3(0.0f, -1.0f,  0.0f))  // -Z
	};

	mat4 projection = Perspective(90.0f, 1.0f, 4.0f, 50.0f);
	glUniformMatrix4fv(glGetUniformLocation(p, "proj"), 1, GL_TRUE, projection);

	mat4 modelMatrix = Translate(translation[0], translation[1], translation[2]) * rotationMatrix * Scale(zoom) * mat4(1.0f);
	glUniformMatrix4fv(glGetUniformLocation(p, "model"), 1, GL_TRUE, modelMatrix);

	glUniformMatrix4fv(glGetUniformLocation(p, "afterChange"), 1, GL_TRUE, afterChange);

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	for (GLuint i = 0; i < 6; ++i) {

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubemapTexture, 0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUniformMatrix4fv(glGetUniformLocation(p, "view"), 1, GL_TRUE, captureViews[i]);		

		newReflec = false;
		renderScene();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

mat4 backup;
float rotationAngle = 0.0f;
float reflectionCubeY = 10.0f;
bool movingUp = true;
float animationSpeed = 1.0f;
float person1RotationAngle = 0.0f;
float person1RotationSpeed = 4.0f;

void push_for_R(vec3 position, float x, float y, float z) {
	backup = afterChange;
	afterChange = Translate(position) * RotateX(x) * RotateY(y) * RotateZ(z) * Translate(-position);
	glUniformMatrix4fv(glGetUniformLocation(p, "afterChange"), 1, GL_TRUE, afterChange);
}

void pop_for_R() {
	afterChange = backup;
	glUniformMatrix4fv(glGetUniformLocation(p, "afterChange"), 1, GL_TRUE, afterChange);
}

void renderScene(void) 
{
	// ToDo

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	vec3 RFcube(0.0f, reflectionCubeY, 0.0f);

	if (newReflec == true) {
		updateCubeMap(cubemapTexture, RFcube, framebuffer, 512, 512);
		newReflec = true;
		glUseProgram(p);
		display(p);
	}
	
	PolyCL();

	push_for_R(vec3(0.0f, 0.5f, 0.0f), 0, rotationAngle, 0);
	addCube(0.0f, 0.5f, 0.0f, 2.0f, 2.0f, 2.0f, currentIndex);
	buffer(vertices, sizeof(vertices), indices, sizeof(indices), slime, sizeof(slime));
	drawPolygon(0, color_tex[0], sizeof(indices));
	pop_for_R();


	PolyCL();
	addCube(0.0f, -1.0f, 0.0f, 20.0f, 1.0f, 20.0f, currentIndex);
	buffer(vertices, sizeof(vertices), indices, sizeof(indices), ground, sizeof(ground));
	drawPolygon(1, color_tex[1], sizeof(indices));

	PolyCL();
	Person(-7.0f, 2.5f, 7.0f, 2.5f);
	buffer(vertices, sizeof(vertices), indices, sizeof(indices), steve, sizeof(steve));
	drawPolygon(2, color_tex[2], sizeof(indices));


	PolyCL();
	push_for_R(vec3(0.0f, 2.5f, -8.5f), 0, person1RotationAngle, 0);
	Person(0.0f, 2.5f, -8.5f, 2.5f);
	buffer(vertices, sizeof(vertices), indices, sizeof(indices), steve, sizeof(steve));
	drawPolygon(3, color_tex[3], sizeof(indices));
	pop_for_R();

	PolyCL();
	Person(7.0f, 2.5f, 7.0f, 2.5f);
	buffer(vertices, sizeof(vertices), indices, sizeof(indices), steve, sizeof(steve));
	drawPolygon(4, color_tex[4], sizeof(indices));


	if (newReflec == true) {
		glUseProgram(q);
		display(q);

		PolyCL();
		addCube(RFcube.x, RFcube.y, RFcube.z, 4.0f, 4.0f, 4.0f, currentIndex);
		bufferR(vertices, sizeof(vertices), indices, sizeof(indices), cubeN, sizeof(cubeN));
		drawPolygon(5, cubemapTexture, sizeof(indices));

		glutSwapBuffers();
	}

}

void idle()
{
	if (animeOn == true) {
		rotationAngle += animationSpeed;
		if (rotationAngle >= 360.0f) {
			rotationAngle -= 360.0f;
		}

		if (person1RotationAngle != 180.0f) {
			person1RotationAngle += person1RotationSpeed;
		}

		if (movingUp) {
			reflectionCubeY += 0.05f;
			if (reflectionCubeY >= 15.0f) {
				movingUp = false;
			}
		}
		else {
			reflectionCubeY -= 0.05f;
			if (reflectionCubeY <= 0.0f) {
				movingUp = true;
			}
		}
	}

	glutPostRedisplay();
}

void LoadTexture(uchar4* dst, int width, int height, GLint color_text, int expand) {
	// Create an RGBA8 2D texture, 24 bit depth texture, 256x256
	glBindTexture(GL_TEXTURE_2D, color_text);
	if (expand == 0) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, dst);
}

void init()
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 1.0); 
	
	// Create shader program
	p = createGLSLProgram("../vshader.vert", NULL, "../fshader.frag"); 
	q = createGLSLProgram("../reflect.vert", NULL, "../reflect.frag");

	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 1.0);
	
	// Load image for texture -- change this code to load different images
	int width, height;
	uchar4 *dst;

	glGenTextures(10, color_tex);

	LoadBMPFile(&dst, &width, &height, "../mob.bmp");
	LoadTexture(dst, width, height, color_tex[0], 0);

	LoadBMPFile(&dst, &width, &height, "../stone2.bmp");
	LoadTexture(dst, width, height, color_tex[1], 0);
	
	LoadBMPFile(&dst, &width, &height, "../steve.bmp");
	LoadTexture(dst, width, height, color_tex[2], 0);

	LoadBMPFile(&dst, &width, &height, "../c3po.bmp");
	LoadTexture(dst, width, height, color_tex[3], 0);

	LoadBMPFile(&dst, &width, &height, "../anakin_papercraft.bmp");
	LoadTexture(dst, width, height, color_tex[4], 0);

	
	cubemapTexture = createCubeMap(512, 512);
	framebuffer = createFrameBuffer();

}

int main(int argc, char **argv) {

	// init GLUT and create Window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(600,600);
	glutCreateWindow("COSE436 - Assignment 3");

	// register callbacks
	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutKeyboardFunc(keyboard);

	glutMouseFunc(mouseButton);
	glutMotionFunc(mouseMotion);

	glutIdleFunc(idle);


	glewInit();
	if (glewIsSupported("GL_VERSION_3_3"))
		printf("Ready for OpenGL 3.3\n");
	else {
		printf("OpenGL 3.3 is not supported\n");
		exit(1);
	}

	init();

	// Entering GLUT event processing cycle
	glutMainLoop();

	glDeleteTextures(10, color_tex);

	return 1;
}