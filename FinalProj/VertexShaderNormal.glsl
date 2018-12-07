#version 450  // GLSL version your computer supports

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal;
layout(location = 3) in vec3 vertexTangent;

out vec2 UV;
out vec3 vertexPositionWorld;
out vec3 TexCoords;
out mat3 TBN;

uniform mat4 PVM;
uniform mat4 modelTransformMatrix;
uniform mat4 projection;
uniform mat4 viewMatrix;

void main(){
	vec4 v = vec4(vertexPosition_modelspace, 1.0);
	vec4 newPosition = modelTransformMatrix * v;
	vec4 projectedPosition = projection * newPosition;
    // Output position of the vertex, in clip space : MVP * position
    gl_Position = PVM * vec4(vertexPosition_modelspace, 1);

	TexCoords = vertexPosition_modelspace;

	vec3 T = normalize(vec3((viewMatrix * modelTransformMatrix) * vec4(vertexTangent, 0.5)));
	vec3 N = normalize(vec3((viewMatrix * modelTransformMatrix) * vec4(vertexNormal, 0.5)));
	//Calculate Bitangent
	T = normalize(T - dot(T,N) * N);
	vec3 B = cross(N,T);
	//TBN Matrix
	TBN = mat3(T, B, N);
	TBN = transpose(TBN);

	vec4 normal_temp = modelTransformMatrix * vec4(vertexNormal, 0);
	//normalWorld = normal_temp.xyz;
	vertexPositionWorld = projectedPosition.xyz;
    // UV of the vertex. No special space for this one.
    UV = vertexUV;
}