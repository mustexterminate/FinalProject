#version 450  // GLSL version your computer supports

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal;
layout(location = 3) in vec3 vertexTangent_modelspace;
layout(location = 4) in vec3 vertexBitangent_modelspace;

out vec2 UV;
out vec3 normalWorld;
out vec3 vertexPositionWorld;
out vec3 TexCoords;
out vec3 EyeDirection_cameraspace;
out vec3 LightDirection_cameraspace;

out vec3 LightDirection_tangentspace;
out vec3 EyeDirection_tangentspace;

uniform mat4 PVM;
uniform mat4 modelTransformMatrix;
uniform mat4 projection;
uniform mat4 V;
uniform mat4 M;
uniform mat3 MV3x3;
uniform vec3 LightPositionWorld;

void main(){
	vec4 v = vec4(vertexPosition_modelspace, 1.0);
	vec4 newPosition = modelTransformMatrix * v;
	vec4 projectedPosition = projection * newPosition;
    // Output position of the vertex, in clip space : MVP * position
    gl_Position = PVM * vec4(vertexPosition_modelspace, 1);

	TexCoords = vertexPosition_modelspace;

	vec4 normal_temp = modelTransformMatrix * vec4(vertexNormal, 0);
	normalWorld = normal_temp.xyz;
	vertexPositionWorld = newPosition.xyz;
    // UV of the vertex. No special space for this one.
    UV = vertexUV;

	vertexNormal_cameraspace = MV3x3 * normalize(vertexNormal);
    vertexTangent_cameraspace = MV3x3 * normalize(vertexTangent_modelspace);
    vertexBitangent_cameraspace = MV3x3 * normalize(vertexBitangent_modelspace);

	mat3 TBN = transpose(mat3(
        vertexTangent_cameraspace,
        vertexBitangent_cameraspace,
        vertexNormal_cameraspace
    ));

	LightDirection_tangentspace = TBN * LightDirection_cameraspace;
    EyeDirection_tangentspace =  TBN * EyeDirection_cameraspace;
}