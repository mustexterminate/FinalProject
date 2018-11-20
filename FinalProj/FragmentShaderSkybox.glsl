#version 450 //GLSL version your computer supports

in vec3 TexCoords;

out vec4 daColor;

uniform samplerCube skybox;

void main()
{
	daColor = texture(skybox, TexCoords);
}
