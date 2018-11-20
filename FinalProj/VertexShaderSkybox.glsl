#version 450  // GLSL version your computer supports

layout(location = 0) in vec3 vertexPosition_modelspace;

out vec3 TexCoords;

uniform mat4 PVM;

void main(){
    gl_Position = PVM * vec4(vertexPosition_modelspace, 1);

	TexCoords = vertexPosition_modelspace;
}