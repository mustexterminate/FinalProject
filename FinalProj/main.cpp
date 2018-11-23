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
#include <tuple>
using namespace std;
using glm::vec3;
using glm::mat4;

float speed = 3.0f;
float mouseSpeed = 1.0f;
//Frame Information
int frame = 0;
//Interaction Variable
float ringRotation = 0.0f;
//
float y_delta = 0.1f;
float x_delta = 0.1f;
float z_delta = 0.1f;
float yRotate_delta = 0.1f;
float zRotate_delta = 0.1f;
int y_rotate_press_num = 0;
int z_press_num = 0;
int x_press_num = 0;
float lightPowerSpec = 0.5f;
float lightPowerDiff = 0.3f;

float lightPositionSpecX = 0.0f;
float lightPositionSpecY = 3.0f;

float eyePositionSpecY = 4.0f;
bool spin = true;
bool mouse = false;
//Mouse Camera
float horizontalAngle = 0.0f;
glm::vec3 cameraPos = glm::vec3(0.0f, 6.0f, 7.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
//
GLuint textInd = 2;
GLint programID;
GLint skyboxID;
GLuint textureID;

//Model Information
GLuint skyboxVAO;
GLuint skyboxVBO;
GLuint VAOs[4];
GLuint vertexBuffers[4];
GLuint uvBuffers[4];
GLuint normalBuffers[4];
GLuint drawSizes[4];
GLuint textures[5];
GLuint skybox_cubemapTexture;

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
	//ProgramID
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
	///
	glLinkProgram(programID);

	if (!checkProgramStatus(programID))
		return;

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);
	
	//SkyboxID
	vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	temp = readShaderCode("VertexShaderSkybox.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(vertexShaderID, 1, adapter, 0);
	temp = readShaderCode("FragmentShaderSkybox.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(fragmentShaderID, 1, adapter, 0);

	glCompileShader(vertexShaderID);
	glCompileShader(fragmentShaderID);

	if (!checkShaderStatus(vertexShaderID) || !checkShaderStatus(fragmentShaderID))
		return;

	skyboxID = glCreateProgram();
	glAttachShader(skyboxID, vertexShaderID);
	glAttachShader(skyboxID, fragmentShaderID);
	///
	glLinkProgram(skyboxID);

	if (!checkProgramStatus(skyboxID))
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
	float cameraSpeed = 10.0f;
	if (key == GLUT_KEY_UP) {
		z_press_num += 10;
	}
	if (key == GLUT_KEY_DOWN) {
		z_press_num -= 10;
	}
	if (key == GLUT_KEY_LEFT) {
		x_press_num += 10;
	}
	if (key == GLUT_KEY_RIGHT) {
		x_press_num -= 10;
	}
}

void PassiveMouse(int x, int y)
{
	glutWarpPointer(800 / 2, 600 / 2);
	horizontalAngle += mouseSpeed * float(800 / 2 - x) / 30;
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

std::tuple<int, int, unsigned char*> loadBMP_data(const char * imagepath) {

	printf("Reading image %s\n", imagepath);

	unsigned char header[54];
	unsigned int dataPos;
	unsigned int imageSize;
	unsigned int width, height;
	unsigned char * data;

	FILE * file = fopen(imagepath, "rb");
	if (!file) { printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", imagepath); getchar(); 
	return std::make_tuple(0, 0, (unsigned char*) 'a');
	}

	if (fread(header, 1, 54, file) != 54) {
		printf("Not a correct BMP file\n");
		return std::make_tuple(0, 0, (unsigned char*) 'a');
	}
	if (header[0] != 'B' || header[1] != 'M') {
		printf("Not a correct BMP file\n");
		return std::make_tuple(0, 0, (unsigned char*) 'a');
	}
	if (*(int*)&(header[0x1E]) != 0) { printf("Not a correct BMP file\n");    
		return std::make_tuple(0, 0, (unsigned char*) 'a');
	}
	if (*(int*)&(header[0x1C]) != 24) { printf("Not a correct BMP file\n");    
		return std::make_tuple(0, 0, (unsigned char*) 'a');
	}

	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);
	if (imageSize == 0)    imageSize = width * height * 3;
	if (dataPos == 0)      dataPos = 54;

	data = new unsigned char[imageSize];
	fread(data, 1, imageSize, file);
	fclose(file);

	return std::make_tuple(width, height, data);
}

GLuint loadCubemap(vector<const GLchar*> faces)
{
	int width, height;
	unsigned char* image;
	GLuint textureID;
	glGenTextures(1, &textureID);
	std::tuple<int, int, unsigned char*> result = std::make_tuple(width, height, image);
	for (GLuint i = 0; i < faces.size(); i++) {
		result = loadBMP_data(faces[i]);
		width = std::get<0>(result);
		height = std::get<1>(result);
		image = std::get<2>(result);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	return textureID;
}

void sendModelData(const char* modelpath, int modelIndex)
{
	//Read .obj file
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	bool res = loadOBJ(modelpath, vertices, uvs, normals);

	//VAO
	glGenVertexArrays(1, &VAOs[modelIndex]);
	glBindVertexArray(VAOs[modelIndex]);
	//VBO
	///Vertex
	glGenBuffers(1, &vertexBuffers[modelIndex]);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers[modelIndex]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers[modelIndex]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	///UVs
	glGenBuffers(1, &uvBuffers[modelIndex]);
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffers[modelIndex]);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffers[modelIndex]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	///Normals
	glGenBuffers(1, &normalBuffers[modelIndex]);
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffers[modelIndex]);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffers[modelIndex]);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	//Size of Vertices
	drawSizes[modelIndex] = (int)vertices.size();
}

void sendDataToOpenGL()
{
	//Sky Box Textures
	vector<const GLchar*> skybox_faces;

	skybox_faces.push_back("sources/texture/skybox/purplenebula_rt.bmp");
	skybox_faces.push_back("sources/texture/skybox/purplenebula_lf.bmp");
	skybox_faces.push_back("sources/texture/skybox/purplenebula_dn.bmp");
	skybox_faces.push_back("sources/texture/skybox/purplenebula_up.bmp");
	skybox_faces.push_back("sources/texture/skybox/purplenebula_bk.bmp");
	skybox_faces.push_back("sources/texture/skybox/purplenebula_ft.bmp");
	skybox_cubemapTexture = loadCubemap(skybox_faces);

	//Other Textures
	textures[0] = loadBMP_custom("sources/texture/Trident_UV.bmp");
	textures[1] = loadBMP_custom("sources/texture/ringTexture.bmp");
	textures[2] = loadBMP_custom("sources/texture/theme1.bmp");
	textures[3] = loadBMP_custom("sources/texture/theme2.bmp");
	textures[4] = loadBMP_custom("sources/texture/theme3.bmp");

	//Skybox Cube
	GLfloat skyboxVertices[] = {
		// positions          
		-50.0f,  50.0f, -50.0f,
		-50.0f, -50.0f, -50.0f,
		50.0f, -50.0f, -50.0f,
		50.0f, -50.0f, -50.0f,
		50.0f,  50.0f, -50.0f,
		-50.0f,  50.0f, -50.0f,

		-50.0f, -50.0f,  50.0f,
		-50.0f, -50.0f, -50.0f,
		-50.0f,  50.0f, -50.0f,
		-50.0f,  50.0f, -50.0f,
		-50.0f,  50.0f,  50.0f,
		-50.0f, -50.0f,  50.0f,

		50.0f, -50.0f, -50.0f,
		50.0f, -50.0f,  50.0f,
		50.0f,  50.0f,  50.0f,
		50.0f,  50.0f,  50.0f,
		50.0f,  50.0f, -50.0f,
		50.0f, -50.0f, -50.0f,

		-50.0f, -50.0f,  50.0f,
		-50.0f,  50.0f,  50.0f,
		50.0f,  50.0f,  50.0f,
		50.0f,  50.0f,  50.0f,
		50.0f, -50.0f,  50.0f,
		-50.0f, -50.0f,  50.0f,

		-50.0f,  50.0f, -50.0f,
		50.0f,  50.0f, -50.0f,
		50.0f,  50.0f,  50.0f,
		50.0f,  50.0f,  50.0f,
		-50.0f,  50.0f,  50.0f,
		-50.0f,  50.0f, -50.0f,

		-50.0f, -50.0f, -50.0f,
		-50.0f, -50.0f,  50.0f,
		50.0f, -50.0f, -50.0f,
		50.0f, -50.0f, -50.0f,
		-50.0f, -50.0f,  50.0f,
		50.0f, -50.0f,  50.0f
	};

	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);
	glBindVertexArray(0);

	//Models
	sendModelData("sources/spaceCraft.obj", 0);
	sendModelData("sources/rock.obj", 1);
	sendModelData("sources/plane.obj", 2);
	sendModelData("sources/Ring.obj", 3);


	textureID = glGetUniformLocation(programID, "myTextureSampler");
}

void paintGL(void)
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glDepthMask(GL_FALSE);

	// Initialize Default Values
	/// Lighting
	vec3 eyePosition(0.0f, eyePositionSpecY, -9.0f);
	vec3 lightPosition(lightPositionSpecX, lightPositionSpecY, 2.0f);
	vec3 ambientLight(0.6f, 0.6f, 0.6f);
	/// Projection & View
	glm::mat4 view = glm::lookAt(vec3((x_delta * x_press_num), 
		6,
		-6 + (z_delta * z_press_num)), //Position
		vec3(0 + (x_delta * x_press_num), 2, 5 + (z_delta * z_press_num)), //Look At
		cameraUp); //Height
	glm::mat4 projection = glm::perspective(glm::radians(80.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::rotate(modelMatrix, glm::radians(horizontalAngle),
		glm::vec3(0, 1, 0));

	glUseProgram(skyboxID);

	//Skybox PVM
	GLuint matrixLocation = glGetUniformLocation(skyboxID, "PVM");

	//
	mat4 PVM = projection * view * modelMatrix;
	glUniformMatrix4fv(matrixLocation, 1, GL_FALSE, &PVM[0][0]);
	// skybox cube
	glDepthMask(GL_FALSE);
	glDepthFunc(GL_LEQUAL);
	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, textures[1]);
	glUniform1i(textureID, 2);

	glUniform1i(glGetUniformLocation(skyboxID, "cubemap"), 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_cubemapTexture);

	glDrawArrays(GL_TRIANGLES, 0, 36);


	glDepthMask(GL_TRUE);

	// programID
	glUseProgram(programID);
	// Get Locations
	/// Lighting
	GLint eyePositionUniformLocation = glGetUniformLocation(programID, "eyePositionWorld");
	GLint lightPositionUniformLocation = glGetUniformLocation(programID, "lightPositionWorld");
	GLint ambientLightUniformLocation = glGetUniformLocation(programID, "ambientLight");
	GLint diffLightUniformLocation = glGetUniformLocation(programID, "lightPowerDiff");
	GLint specLightUniformLocation = glGetUniformLocation(programID, "lightPowerSpec");
	/// PVM
	GLint transformationMatrixLocation = glGetUniformLocation(programID, "modelTransformMatrix");
	matrixLocation = glGetUniformLocation(programID, "PVM");

	// Bind Defaults
	/// Lighting
	glUniform3fv(eyePositionUniformLocation, 1, &eyePosition[0]);
	glUniform3fv(lightPositionUniformLocation, 1, &lightPosition[0]);
	glUniform3fv(ambientLightUniformLocation, 1, &ambientLight[0]);
	glUniform1f(diffLightUniformLocation, lightPowerDiff);
	glUniform1f(specLightUniformLocation, lightPowerSpec);

	//Keeping this For Reference, to be removed when submitting [allows us to tell where the x & z axis expand to]
	// Model 3: Plane
	glBindVertexArray(VAOs[2]);
	/// Transformation
	PVM = projection * view * modelMatrix;
	glUniformMatrix4fv(transformationMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);
	glUniformMatrix4fv(matrixLocation, 1, GL_FALSE, &PVM[0][0]);
	/// Texture
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, textures[textInd]);
	glUniform1i(textureID, 2);
	/// Draw
	glDrawArrays(GL_TRIANGLES, 0, drawSizes[2]);

	// Model 1: SpaceCraft
	glBindVertexArray(VAOs[0]);
	/// Transformation
	mat4 modelATransformMatrix = modelATransformMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(horizontalAngle),
		glm::vec3(0, 1, 0)); 
	modelATransformMatrix = glm::translate(glm::mat4(1.0f),
		glm::vec3(x_delta * x_press_num, 5.0f, z_delta * z_press_num));
	modelATransformMatrix = glm::scale(modelATransformMatrix, vec3(0.003f));
	
	
	
	glUniformMatrix4fv(transformationMatrixLocation, 1, GL_FALSE, &modelATransformMatrix[0][0]);
	PVM = projection * view * modelATransformMatrix;
	glUniformMatrix4fv(matrixLocation, 1, GL_FALSE, &PVM[0][0]);
	/// Texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glUniform1i(textureID, 0);
	/// Draw
	glDrawArrays(GL_TRIANGLES, 0, drawSizes[0]);

	// Model 2: Ring
	glBindVertexArray(VAOs[3]);
	/// Texture
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textures[1]);
	glUniform1i(textureID, 1);
	/// Transformation
	mat4 modelBTranslate = glm::translate(mat4(1.0f), glm::vec3(0.0f, 6.0f, 0.0f));
	mat4 modelBScale = glm::scale(mat4(1.0f), glm::vec3(0.0625));
	mat4 modelBRotate = glm::rotate(mat4(1.0f), glm::radians(90.0f), vec3(0, 0, 1));
	modelBRotate = glm::rotate(modelBRotate, glm::radians(frame * y_delta * 3), vec3(1, 0, 0));

	mat4 modelBTransformMatrix = modelBTranslate * modelBScale * modelBRotate; //Rotate --> Scale --> Translate

	glUniformMatrix4fv(transformationMatrixLocation, 1, GL_FALSE, &modelBTransformMatrix[0][0]);
	PVM = projection * view * modelBTransformMatrix;
	glUniformMatrix4fv(matrixLocation, 1, GL_FALSE, &PVM[0][0]);
	/// Draw
	glDrawArrays(GL_TRIANGLES, 0, drawSizes[3]);
	
	glFlush();
	glutPostRedisplay();

	frame++;
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