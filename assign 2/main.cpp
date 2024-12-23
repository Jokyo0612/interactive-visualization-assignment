/*
 * Skeleton code for COSE436 Fall 2024 Assignment 2
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
GLuint p[3];

// read mesh data
GLuint vao, vboVertices, vboNormals, ibo;
vec3* vertices = nullptr;
vec3* normals = nullptr;
unsigned int* indices = nullptr;
int vertexCount = 0;
int faceCount = 0;
int index = 0;

// choose shader
int NP = 0;
int up = 0;

// shilluate & toon
float thick = 0.1;
int numShades = 4;

// light component
float alpha = 10.0;
int numLights = 0;
float Kd = 1.0;
float Ka = 0.5;
float Ks = 0.5;

struct Light {
	vec3 position;	
	vec3 Ld;
	vec3 La;
	vec3 Ls;
};

Light lights[5];

void lightArray() {
	lights[0].position = vec3(3.0f, 5.0f, 3.0f);  // 카메라 위치에서 조금 위쪽에 배치
	lights[0].Ld = vec3(1.8f, 1.2f, 0.8f);  // 강한 흰색 확산광
	lights[0].La = vec3(0.4f, 0.4f, 0.4f);  // 부드러운 흰색 주변광
	lights[0].Ls = vec3(0.6f, 0.5f, 0.6f);  // 선명한 하이라이트

	// Light Source 1: 보조 블루 조명 (주변 조명 강조)
	lights[1].position = vec3(3.0f, 10.0f, 3.0f); // 위쪽 중앙에 위치
	lights[1].Ld = vec3(0.3f, 0.3f, 1.0f); // 파란색 확산광
	lights[1].La = vec3(0.1f, 0.1f, 0.3f); // 파란색 주변광
	lights[1].Ls = vec3(0.4f, 0.4f, 1.0f); // 파란색 반사광

	// Light Source 2: 탑 라이트 (분위기 조성용 조명)
	lights[2].position = vec3(0.0f, 10.0f, -5.0f);  // 위쪽에서 아래를 비추는 조명
	lights[2].Ld = vec3(1.0f, 0.8f, 0.6f);  // 따뜻한 연한 주황색 확산광
	lights[2].La = vec3(0.3f, 0.2f, 0.1f);  // 부드러운 붉은색 주변광
	lights[2].Ls = vec3(0.8f, 0.7f, 0.5f);  // 붉은 반사광

	// Light Source 3: 필 라이트 (그린 조명)
	lights[3].position = vec3(-3.0f, -10.0f, -3.0f); // 아래쪽 중앙에 위치
	lights[3].Ld = vec3(0.8f, 0.2f, 0.4f); // 녹색 확산광
	lights[3].La = vec3(0.4f, 0.2f, 0.2f); // 녹색 주변광
	lights[3].Ls = vec3(0.5f, 1.0f, 0.5f); // 녹색 반사광

	// Light Source 4: 백 라이트 (강한 대비용 조명)
	lights[4].position = vec3(-5.0f, 4.0f, -6.0f);  // 카메라 왼쪽 위쪽에 배치
	lights[4].Ld = vec3(0.9f, 0.3f, 0.3f);  // 강렬한 붉은색 확산광
	lights[4].La = vec3(0.3f, 0.1f, 0.1f);  // 붉은색 주변광
	lights[4].Ls = vec3(0.6f, 0.5f, 0.5f);  // 강한 붉은 반사광
}

void loadOFFFile(const char* filename) {
	FILE* file = fopen(filename, "r");
	if (!file) {
		std::cerr << "Error opening file: " << filename << std::endl;
		return;
	}

	char header[4];
	fscanf(file, "%s", header);
	if (std::string(header) != "OFF") {
		std::cerr << "Not an OFF file!" << std::endl;
		fclose(file);
		return;
	}

	fscanf(file, "%d %d %*d", &vertexCount, &faceCount);
	vertices = new vec3[vertexCount];
	normals = new vec3[vertexCount]();
	indices = new unsigned int[faceCount * 3];

	// vertex
	for (int i = 0; i < vertexCount; i++) {
		fscanf(file, "%f %f %f", &vertices[i].x, &vertices[i].y, &vertices[i].z);
	}

	// indicies
	for (int i = 0; i < faceCount; i++) {
		int v1, v2, v3;
		fscanf(file, "%*d %d %d %d", &v1, &v2, &v3);

		indices[i * 3] = v1;
		indices[i * 3 + 1] = v2;
		indices[i * 3 + 2] = v3;

		// calculate face normal
		vec3 edge1 = vertices[v2] - vertices[v1];
		vec3 edge2 = vertices[v3] - vertices[v1];
		vec3 faceNormal = normalize(cross(edge1, edge2));

		normals[v1] += faceNormal;
		normals[v2] += faceNormal;
		normals[v3] += faceNormal;
	}

	// normal vertex (average face normal)
	for (int i = 0; i < vertexCount; ++i) {
		normals[i] = normalize(normals[i]);
	}

	fclose(file);
}

void buffer() {

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Vertex Buffer Object (VBO) for vertices
	glGenBuffers(1, &vboVertices);
	glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexCount * 3, vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	// Vertex Buffer Object (VBO) for normals
	glGenBuffers(1, &vboNormals);
	glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexCount * 3, normals, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);

	// Index Buffer Object (IBO)
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * faceCount * 3, indices, GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void changeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if(h == 0) h = 1;

	float ratio = 1.0f * (float) w / (float)h;

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);
		
}

void keyboard(unsigned char key, int x, int y)
{
	// ToDo
	if(key == 'p') {
		NP = (NP + 1) % 2;
	}
	// Polygon Mesh Change
	if (key == 'm') {
		index = (index + 1) % 3;
		up = 1;
	}
	if (key == 's') {
		if (NP == 0) {
			numLights = (numLights + 1) % 3;
		}
	}
	// shiness param & silluate thickness
	if (key == '+') {
		if (NP == 0) {
			alpha += 10;
			printf("%f\n", alpha);
		}
		else {
			thick += 0.1;
			printf("%f\n", thick);
		}
	}
	if (key == '-') {
		if (NP == 0) {
			alpha -= 10;
			printf("%f\n", alpha);
		}
		else {
			thick -= 0.1;
			printf("%f\n", thick);
		}
	}
	// DAS param
	if (key == '1') {
		if (NP == 0) {
			Kd = Kd - 0.1;
			if (Kd < 0) {
				Kd = 0;
			}
			printf("%f\n", Kd);
		}
		
	}
	if (key == '2') {
		if (NP != 0) {
			numShades = 2;
		}
	}
	if (key == '3') {
		if (NP == 0) {
			Kd = Kd + 0.1;
			if (Kd > 1.0) {
				Kd = 1.0;
			}
			printf("%f\n", Kd);
		}
		else {
			numShades = 3;
		}
		
	}
	if (key == '4') {
		if (NP == 0) {
			Ka = Ka - 0.1;
			if (Ka < 0) {
				Ka = 0;
			}
			printf("%f\n", Ka);
		}
		else {
			numShades = 4;
		}
	}
	if (key == '5') {
		if (NP != 0) {
			numShades = 5;
		}
	}
	if (key == '6') {
		if (NP == 0) {
			Ka = Ka + 0.1;
			if (Ka > 1.0) {
				Ka = 1.0;
			}
			printf("%f\n", Ka);
		}
		else {
			numShades = 6;
		}
		
	}
	if (key == '7') {
		if (NP == 0) {
			Ks = Ks - 0.1;
			if (Ks < 0) {
				Ks = 0;
			}
			printf("%f\n", Ks);
		}
		else {
			numShades = 7;
		}
	}
	if (key == '8') {
		if (NP != 0) {
			numShades = 8;
		}
	}
	if (key == '9') {
		if (NP == 0) {
			Ks = Ks + 0.1;
			if (Ks > 1.0) {
				Ks = 1.0;
			}
			printf("%f\n", Ks);
		}
		else {
			numShades = 9;
		}
	}


	glutPostRedisplay();
}

void update() {
	if (up == 1) {
		if (index == 0) {
			loadOFFFile("../mesh-data/bunny.off");
			buffer();
		}
		else if (index == 1) {
			printf("plaeas wait to chage\n");
			loadOFFFile("../mesh-data/dragon-full.off");
			printf("done\n");
			buffer();
		}
		else if (index == 2) {
			loadOFFFile("../mesh-data/fandisk.off");
			buffer();
		}
	}
	up = 0;
}

void display() {

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	vec4 eye;
	float currScale;

	mat4 model = mat4(1.0f);

	if (index == 2) {
		eye = vec4(5.0, 3.0, -5.0, 1.0);
		currScale = 20.0f;
	}
	else {
		eye = vec4(1.0, 5.0, 15.0, 1.0);
		currScale = 150.0f;

		mat4 trans = Translate(0.0f, -10.0f, 0.0f);
		model = trans * model;
	}
	
	vec4 at(0.0, 0.0, 0.0, 1.0);
	vec4 up(0.0, 1.0, 0.0, 0.0);
	mat4 viewMatrix = LookAt(eye, at, up);

	mat4 modelViewMatrix = viewMatrix * model;
	mat4 projMatrix = Ortho(-30.0f, 30.0f, -30.0f, 30.0f, 0.1f, 100.0f);

	GLuint modelMatrixLoc = glGetUniformLocation(p[NP], "modelViewMatrix");
	glUniformMatrix4fv(modelMatrixLoc, 1, GL_TRUE, modelViewMatrix);

	GLuint projLoc = glGetUniformLocation(p[NP], "projectionMatrix");
	glUniformMatrix4fv(projLoc, 1, GL_TRUE, projMatrix);

	GLuint scaleLocation = glGetUniformLocation(p[NP], "m_Scale");
	if (scaleLocation != -1) glUniform1f(scaleLocation, currScale);

}

void lightSRC() {

	vec3 attenuation = vec3(1.0, 0.1, 0.01);

	GLuint kdLoc = glGetUniformLocation(p[NP], "Kd");
	GLuint kaLoc = glGetUniformLocation(p[NP], "Ka");
	GLuint ksLoc = glGetUniformLocation(p[NP], "Ks");
	
	GLuint att = glGetUniformLocation(p[NP], "attenuation");
	GLuint a1 = glGetUniformLocation(p[NP], "alpha");

	glUniform1i(glGetUniformLocation(p[NP], "numLights"), numLights);

	int j = numLights % 2;
	for (int i = 0; i <= numLights; i++) {
		char uniformName[32];

		sprintf(uniformName, "lightpos[%d]", i);
		GLuint lightPosLoc = glGetUniformLocation(p[NP], uniformName);

		sprintf(uniformName, "Ld[%d]", i);
		GLuint ldLoc = glGetUniformLocation(p[NP], uniformName);

		sprintf(uniformName, "La[%d]", i);
		GLuint laLoc = glGetUniformLocation(p[NP], uniformName);

		sprintf(uniformName, "Ls[%d]", i);
		GLuint lsLoc = glGetUniformLocation(p[NP], uniformName);

		glUniform3fv(lightPosLoc, 1, &lights[j].position[0]);
		glUniform3fv(ldLoc, 1, &lights[j].Ld[0]);
		glUniform3fv(laLoc, 1, &lights[j].La[0]);
		glUniform3fv(lsLoc, 1, &lights[j].Ls[0]);

		j = j + 2;
	}

	glUniform1f(kdLoc, Kd);
	glUniform1f(kaLoc, Ka);
	glUniform1f(ksLoc, Ks);
	
	glUniform3fv(att, 1, &attenuation[0]);
	glUniform1f(a1, alpha);
}

void toon() {
	NP = 2;
	vec3 direct = vec3(0.5f, 0.0f, 2.5f);
	vec4 base = vec4(1.0f, 0.6f, 0.4f);

	glUseProgram(p[NP]);
	
	display();
	
	GLuint direcLoc = glGetUniformLocation(p[NP], "direct");
	glUniform3fv(direcLoc, 1, &direct[0]);

	GLuint baseLoc = glGetUniformLocation(p[NP], "base");
	glUniform4fv(baseLoc, 1, &base[0]);
	
	GLuint shadeLoc = glGetUniformLocation(p[NP], "numShades");
	glUniform1i(shadeLoc, numShades);
}

void silluate() {
	if (NP == 1) {
		GLuint thickLoc = glGetUniformLocation(p[NP], "thick");
		glUniform1f(thickLoc, thick);

		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CCW);

		glCullFace(GL_FRONT);
		glDrawElements(GL_TRIANGLES, faceCount * 3, GL_UNSIGNED_INT, 0);

		glCullFace(GL_BACK);
		float i = 0.0f;
		glUniform1f(thickLoc, i);

		toon();

		NP = 1;
	}
}

void renderScene(void) 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(p[NP]);

	display();	// model view
	update();   // polygon select

	if (NP == 0) {
		lightSRC(); // light source
	}

	//glutSolidTeapot(0.5);
	//glColor3f(1, 1, 0);

	glBindVertexArray(vao);

	silluate();

	glDrawElements(GL_TRIANGLES, faceCount * 3, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glDisable(GL_CULL_FACE);

	glutSwapBuffers();
}

void idle()
{
	glutPostRedisplay();
}


void init()
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 1.0); 
	
	// Create shader program
	p[0] = createGLSLProgram( "../phong.vert", NULL, "../phong.frag" ); // Phong
	p[1] = createGLSLProgram( "../silhouette.vert", NULL, "../silhouette.frag" ); // Silhouette
	p[2] = createGLSLProgram( "../toon.vert", NULL, "../toon.frag" ); // Cartoon
	
	loadOFFFile("../mesh-data/bunny.off");
	buffer();

	lightArray();

	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 1.0);
}

int main(int argc, char **argv) {

	// init GLUT and create Window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(600,600);
	glutCreateWindow("COSE436 - Assignment 2");

	// register callbacks
	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutKeyboardFunc(keyboard);
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