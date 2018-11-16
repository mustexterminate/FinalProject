/*********************************************************
FILE : main.cpp (csci3260 2018-2019 Final Project)
*********************************************************/
/*********************************************************
Student Information
Student ID: 1155123051\1155123323
Student Name: Alexander Yoo\Ernest Tran
*********************************************************/

#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\freeglut.h"
#include "Dependencies\glm\glm.hpp"
#include "Dependencies\glm\gtc\matrix_transform.hpp"
#include "Dependencies\glm\gtc\type_ptr.hpp"
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;
using glm::vec3;
using glm::mat4;

glm::vec3 cameraPosition = glm::vec3(0, 9, -9);
glm::vec3 cameraFront = glm::vec3(0, 0, -1);
glm::vec3 cameraUp = glm::vec3(0, 1, 0);
glm::mat4 modelBTransformMatrix = glm::translate(glm::mat4(),
	glm::vec3(6, 3, 0.0f));

float horizontalAngle = 3.14f;
float verticalAngle = 0.0f;

float speed = 3.0f;
float mouseSpeed = 0.02f;
//Interaction Variable
float y_delta = 0.1f;
float z_delta = 0.1f;
float yRotate_delta = 0.1f;
float zRotate_delta = 0.1f;
int y_rotate_press_num = 0;
int z_press_num = 0;
float lightPowerSpec = 0.5f;
float lightPowerDiff = 0.3f;

float lightPositionSpecX = 0.0f;
float lightPositionSpecY = 3.0f;

float eyePositionSpecY = 4.0f;
bool spin = true;
bool mouse = false;

//
GLuint textInd = 2;
GLint programID;
GLuint textureID;

//Model Information
GLuint VAOs[3];
GLuint vertexBuffers[3];
GLuint uvBuffers[3];
GLuint normalBuffers[3];
GLuint drawSizes[3];
GLuint textures[5];

//a series utilities for setting shader parameters 
void setMat4(const std::string &name, glm::mat4& value)
{
	unsigned int transformLoc = glGetUniformLocation(programID, name.c_str());
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(value));
}

void setVec4(const std::string &name, glm::vec4 value)
{
	glUniform4fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
}
void setVec3(const std::string &name, glm::vec3 value)
{
	glUniform3fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
}
void setFloat(const std::string &name, float value)
{
	glUniform1f(glGetUniformLocation(programID, name.c_str()), value);
}
void setInt(const std::string &name, int value)
{
	glUniform1i(glGetUniformLocation(programID, name.c_str()), value);
}

bool checkStatus(
	GLuint objectID,
	PFNGLGETSHADERIVPROC objectPropertyGetterFunc,
	PFNGLGETSHADERINFOLOGPROC getInfoLogFunc,
	GLenum statusType)
{
	GLint status;
	objectPropertyGetterFunc(objectID, statusType, &status);
	if (status != GL_TRUE)
	{
		GLint infoLogLength;
		objectPropertyGetterFunc(objectID, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar* buffer = new GLchar[infoLogLength];

		GLsizei bufferSize;
		getInfoLogFunc(objectID, infoLogLength, &bufferSize, buffer);
		cout << buffer << endl;

		delete[] buffer;
		return false;
	}
	return true;
}

bool checkShaderStatus(GLuint shaderID)
{
	return checkStatus(shaderID, glGetShaderiv, glGetShaderInfoLog, GL_COMPILE_STATUS);
}

bool checkProgramStatus(GLuint programID)
{
	return checkStatus(programID, glGetProgramiv, glGetProgramInfoLog, GL_LINK_STATUS);
}

string readShaderCode(const char* fileName)
{
	ifstream meInput(fileName);
	if (!meInput.good())
	{
		cout << "File failed to load..." << fileName;
		exit(1);
	}
	return std::string(
		std::istreambuf_iterator<char>(meInput),
		std::istreambuf_iterator<char>()
	);
}

void installShaders()
{
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	const GLchar* adapter[1];
	string temp = readShaderCode("VertexShaderCode.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(vertexShaderID, 1, adapter, 0);
	temp = readShaderCode("FragmentShaderCode.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(fragmentShaderID, 1, adapter, 0);

	glCompileShader(vertexShaderID);
	glCompileShader(fragmentShaderID);

	if (!checkShaderStatus(vertexShaderID) || !checkShaderStatus(fragmentShaderID))
		return;

	programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);

	if (!checkProgramStatus(programID))
		return;

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	glUseProgram(programID);
}

void keyboard(unsigned char key, int x, int y)
{
	if (key == 's') {
		spin = !spin;
	}
	if (key == '1') {
		textInd = 2;
	}
	if (key == '2') {
		textInd = 3;
	}
	if (key == '3') {
		textInd = 4;
	}
	if (key == ' ') {
		mouse = !mouse;
	}
	if (key == 'q') {
		lightPowerDiff += 0.1;
	}
	if (key == 'w') {
		lightPowerDiff -= 0.1;
	}
	if (key == 'z') {
		lightPowerSpec += 0.1;
	}
	if (key == 'x') {
		lightPowerSpec -= 0.1;
	}
	if (key == 'j') {
		lightPositionSpecX += 0.1;
	}
	if (key == 'l') {
		lightPositionSpecX -= 0.1;
	}
	if (key == 'i') {
		lightPositionSpecY += 0.1;
	}
	if (key == 'k') {
		lightPositionSpecY -= 0.1;
	}
	if (key == 'n') {
		eyePositionSpecY += 0.1;
	}
	if (key == 'm') {
		eyePositionSpecY -= 0.1;
	}
}

void move(int key, int x, int y) 
{
	if (key == GLUT_KEY_UP) {
		z_press_num += 10;
	}
	if (key == GLUT_KEY_DOWN) {
		z_press_num -= 10;
	}
	if (key == GLUT_KEY_LEFT) {
		y_rotate_press_num += 20;
	}
	if (key == GLUT_KEY_RIGHT) {
		y_rotate_press_num -= 20;
	}
}

void PassiveMouse(int x, int y)
{
	if (mouse) {
		glutWarpPointer(800 / 2, 600 / 2);
		horizontalAngle += mouseSpeed * float(800 / 2 - x) / 30;
		verticalAngle += mouseSpeed * float(600 / 2 - y) / 30;
	}
	
}

bool loadOBJ(
	const char * path,
	std::vector<glm::vec3> & out_vertices,
	std::vector<glm::vec2> & out_uvs,
	std::vector<glm::vec3> & out_normals
) {
	printf("Loading OBJ file %s...\n", path);

	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;


	FILE * file = fopen(path, "r");
	if (file == NULL) {
		printf("Impossible to open the file ! Are you in the right path ? See Tutorial 6 for details\n");
		getchar();
		return false;
	}

	while (1) {

		char lineHeader[128];
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

				   // else : parse lineHeader

		if (strcmp(lineHeader, "v") == 0) {
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			uv.y = -uv.y; 
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9) {
				printf("File can't be read by our simple parser :-( Try exporting with other options\n");
				return false;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
		else {
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}

	}

	// For each vertex of each triangle
	for (unsigned int i = 0; i<vertexIndices.size(); i++) {

		// Get the indices of its attributes
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int uvIndex = uvIndices[i];
		unsigned int normalIndex = normalIndices[i];

		// Get the attributes thanks to the index
		glm::vec3 vertex = temp_vertices[vertexIndex - 1];
		glm::vec2 uv = temp_uvs[uvIndex - 1];
		glm::vec3 normal = temp_normals[normalIndex - 1];

		// Put the attributes in buffers
		out_vertices.push_back(vertex);
		out_uvs.push_back(uv);
		out_normals.push_back(normal);

	}

	return true;
}

GLuint loadBMP_custom(const char * imagepath) {

	printf("Reading image %s\n", imagepath);

	unsigned char header[54];
	unsigned int dataPos;
	unsigned int imageSize;
	unsigned int width, height;
	unsigned char * data;

	FILE * file = fopen(imagepath, "rb");
	if (!file) { printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", imagepath); getchar(); return 0; }

	if (fread(header, 1, 54, file) != 54) {
		printf("Not a correct BMP file\n");
		return 0;
	}
	if (header[0] != 'B' || header[1] != 'M') {
		printf("Not a correct BMP file\n");
		return 0;
	}
	if (*(int*)&(header[0x1E]) != 0) { printf("Not a correct BMP file\n");    return 0; }
	if (*(int*)&(header[0x1C]) != 24) { printf("Not a correct BMP file\n");    return 0; }

	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);
	if (imageSize == 0)    imageSize = width*height * 3; 
	if (dataPos == 0)      dataPos = 54; 

	data = new unsigned char[imageSize];
	fread(data, 1, imageSize, file);
	fclose(file);
	
	GLuint textureID;
	
	glGenTextures(1, &textureID);

	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	delete[] data;
	return textureID;
}

void sendDataToOpenGL()
{
	glBindVertexArray(01);

	std::vector< glm::vec3 > verticesA;
	std::vector< glm::vec2 > uvsA;
	std::vector< glm::vec3 > normalsA;
	std::vector< glm::vec3 > verticesB;
	std::vector< glm::vec2 > uvsB;
	std::vector< glm::vec3 > normalsB;
	std::vector< glm::vec3 > verticesC;
	std::vector< glm::vec2 > uvsC;
	std::vector< glm::vec3 > normalsC;

	textures[0] = loadBMP_custom("sources/texture/camo.bmp");
	textures[1] = loadBMP_custom("sources/texture/camo.bmp");
	textures[2] = loadBMP_custom("sources/texture/theme1.bmp");
	textures[3] = loadBMP_custom("sources/texture/theme2.bmp");
	textures[4] = loadBMP_custom("sources/texture/theme3.bmp");

	//Model 1
	bool res = loadOBJ("sources/jeep.obj", verticesA, uvsA, normalsA);
	glGenVertexArrays(1, &VAOs[0]);
	glBindVertexArray(VAOs[0]);

	glGenBuffers(1, &vertexBuffers[0]);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers[0]);
	glBufferData(GL_ARRAY_BUFFER, verticesA.size() * sizeof(glm::vec3), &verticesA[0], GL_STATIC_DRAW);

	drawSizes[0] = verticesA.size();

	glGenBuffers(1, &uvBuffers[0]);
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffers[0]);
	glBufferData(GL_ARRAY_BUFFER, uvsA.size() * sizeof(glm::vec2), &uvsA[0], GL_STATIC_DRAW);

	glGenBuffers(1, &normalBuffers[0]);
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffers[0]);
	glBufferData(GL_ARRAY_BUFFER, normalsA.size() * sizeof(glm::vec3), &normalsA[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers[0]);
	glVertexAttribPointer(
		0,                  // attribute
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized
		0,                  // stride
		(void*)0            // array buffer offset
	);
	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffers[0]);
	glVertexAttribPointer(
		1,                                // attribute
		2,                                // size : U+V => 2
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffers[0]);
	glVertexAttribPointer(
		2,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	//Model 2
	res = loadOBJ("sources/rock.obj", verticesB, uvsB, normalsB);
	glGenVertexArrays(1, &VAOs[1]);
	glBindVertexArray(VAOs[1]);

	glGenBuffers(1, &vertexBuffers[1]);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers[1]);
	glBufferData(GL_ARRAY_BUFFER, verticesB.size() * sizeof(glm::vec3), &verticesB[0], GL_STATIC_DRAW);

	drawSizes[1] = verticesB.size();

	glGenBuffers(1, &uvBuffers[1]);
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffers[1]);
	glBufferData(GL_ARRAY_BUFFER, uvsB.size() * sizeof(glm::vec2), &uvsB[0], GL_STATIC_DRAW);

	glGenBuffers(1, &normalBuffers[1]);
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffers[1]);
	glBufferData(GL_ARRAY_BUFFER, normalsB.size() * sizeof(glm::vec3), &normalsB[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers[1]);
	glVertexAttribPointer(
		0,                  // attribute. 
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized
		0,                  // stride
		(void*)0            // array buffer offset
	);
	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffers[1]);
	glVertexAttribPointer(
		1,                                // attribute
		2,                                // size : U+V => 2
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized
		0,                                // stride
		(void*)0                          // array buffer offset
	);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffers[1]);
	glVertexAttribPointer(
		2,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	//Model 3
	res = loadOBJ("sources/Plane.obj", verticesC, uvsC, normalsC);
	glGenVertexArrays(1, &VAOs[2]);
	glBindVertexArray(VAOs[2]);

	glGenBuffers(1, &vertexBuffers[2]);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers[2]);
	glBufferData(GL_ARRAY_BUFFER, verticesC.size() * sizeof(glm::vec3), &verticesC[0], GL_STATIC_DRAW);

	drawSizes[2] = verticesC.size();

	glGenBuffers(1, &uvBuffers[2]);
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffers[2]);
	glBufferData(GL_ARRAY_BUFFER, uvsC.size() * sizeof(glm::vec2), &uvsC[0], GL_STATIC_DRAW);

	glGenBuffers(1, &normalBuffers[2]);
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffers[2]);
	glBufferData(GL_ARRAY_BUFFER, normalsC.size() * sizeof(glm::vec3), &normalsC[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers[2]);
	glVertexAttribPointer(
		0,                  // attribute.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized
		0,                  // stride
		(void*)0            // array buffer offset
	);
	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffers[2]);
	glVertexAttribPointer(
		1,                                // attribute
		2,                                // size : U+V => 2
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized
		0,                                // stride
		(void*)0                          // array buffer offset
	);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffers[2]);
	glVertexAttribPointer(
		2,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	textureID = glGetUniformLocation(programID, "myTextureSampler");
}

void paintGL(void)
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	
	//Eye Position
	GLint eyePositionUniformLocation = glGetUniformLocation(programID, "eyePositionWorld");
	vec3 eyePosition(0.0f, eyePositionSpecY, -9.0f);
	glUniform3fv(eyePositionUniformLocation, 1, &eyePosition[0]);

	//Light Position
	GLint lightPositionUniformLocation = glGetUniformLocation(programID, "lightPositionWorld");
	vec3 lightPosition(lightPositionSpecX, lightPositionSpecY, 2.0f);
	glUniform3fv(lightPositionUniformLocation, 1, &lightPosition[0]);

	//Projection
	glm::mat4 projection = glm::perspective(glm::radians(80.0f), 4.0f/3.0f, 0.1f, 100.0f);
	GLint projectionLocation = glGetUniformLocation(programID, "projection");
	glUniformMatrix3fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

	// Camera world space location
	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle), //X
		sin(verticalAngle), //Y
		cos(verticalAngle) * cos(horizontalAngle) //Z
	);
	glm::mat4 view = glm::lookAt(cameraPosition, cameraFront + direction, cameraUp);
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	glm::mat4 modelATransformMatrix = glm::mat4(1.0f);
	GLint modelMatrixUniformLocation =
		glGetUniformLocation(programID, "modelMatrix");

	// Ambient Light Color
	GLint ambientLightUniformLocation = glGetUniformLocation(programID, "ambientLight");
	vec3 ambientLight(0.6f, 0.6f, 0.6f);
	glUniform3fv(ambientLightUniformLocation, 1, &ambientLight[0]);

	// Light for Diffuse and Specular power
	GLint diffLightUniformLocation = glGetUniformLocation(programID, "lightPowerDiff");
	glUniform1f(diffLightUniformLocation, lightPowerDiff);
	GLint specLightUniformLocation = glGetUniformLocation(programID, "lightPowerSpec");
	glUniform1f(specLightUniformLocation, lightPowerSpec);

	// Matrix Transformation
	modelATransformMatrix = glm::translate(glm::mat4(),
		glm::vec3(0.0f, 0.0f, z_delta * z_press_num));
	modelATransformMatrix = glm::rotate(modelATransformMatrix, glm::radians(yRotate_delta*y_rotate_press_num),
		glm::vec3(0, 1, 0));
	GLint transformationMatrixLocation = glGetUniformLocation(programID, "modelTransformMatrix");
	glUniformMatrix4fv(transformationMatrixLocation, 1, GL_FALSE, &modelATransformMatrix[0][0]);

	//Matrix
	glm::mat4 PVM = projection * view * modelATransformMatrix;
	GLuint matrixLocation = glGetUniformLocation(programID, "PVM");
	glUniformMatrix4fv(matrixLocation, 1, GL_FALSE, &PVM[0][0]);

	//Model 1
	glBindVertexArray(01);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glUniform1i(textureID, 0);
	glDrawArrays(GL_TRIANGLES, 0, drawSizes[0]);

	//Model 2
	glBindVertexArray(02);
	if (spin) {
		modelBTransformMatrix = glm::rotate(modelBTransformMatrix, glm::radians(yRotate_delta * 3),
			glm::vec3(0, 1, 0));
	}
	glUniformMatrix4fv(transformationMatrixLocation, 1, GL_FALSE, &modelBTransformMatrix[0][0]);
	PVM = projection * view * modelBTransformMatrix;
	glUniformMatrix4fv(matrixLocation, 1, GL_FALSE, &PVM[0][0]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textures[1]);
	glUniform1i(textureID, 1);
	glDrawArrays(GL_TRIANGLES, 0, drawSizes[1]);

	//Model 3
	glBindVertexArray(03);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, textures[textInd]);
	glUniform1i(textureID, 2);
	PVM = projection * view * modelMatrix;
	glUniformMatrix4fv(transformationMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);
	glUniformMatrix4fv(matrixLocation, 1, GL_FALSE, &PVM[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, drawSizes[2]);
	glFlush();
	glutPostRedisplay();
}

void initializedGL(void) //run only once
{
	glewInit();
	installShaders();
	sendDataToOpenGL();
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutCreateWindow("Assignment 2");
	glutReshapeWindow(800, 600);
	//TODO:
	initializedGL();
	glutDisplayFunc(paintGL);
	
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(move);
	glutPassiveMotionFunc(PassiveMouse);
	
	glutMainLoop();

	return 0;
}