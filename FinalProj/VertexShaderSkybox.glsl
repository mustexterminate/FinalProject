#version 330

layout (location = 0) in vec3 position;
//Texture Coordinates
out vec3 TexCoords;
//Transformation
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelTransformMatrix;

void main()
{
	TexCoords = position;

	vec4 pos = projectionMatrix * viewMatrix * modelTransformMatrix * vec4(position, 1.0);
	gl_Position = pos;
}