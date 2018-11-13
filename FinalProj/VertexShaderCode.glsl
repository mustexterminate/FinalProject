#version 450  // GLSL version your computer supports

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal;

out vec2 UV;
out vec3 normalWorld;
out vec3 vertexPositionWorld;

uniform mat4 PVM;
uniform mat4 modelTransformMatrix;
uniform mat4 projection;
void main(){
	vec4 v = vec4(vertexPosition_modelspace, 1.0);
	vec4 newPosition = modelTransformMatrix * v;
	vec4 projectedPosition = projection * newPosition;
    // Output position of the vertex, in clip space : MVP * position
    gl_Position = PVM * vec4(vertexPosition_modelspace, 1);
	
	vec4 normal_temp = modelTransformMatrix * vec4(vertexNormal, 0);
	normalWorld = normal_temp.xyz;
	vertexPositionWorld = newPosition.xyz;
    // UV of the vertex. No special space for this one.
    UV = vertexUV;
}