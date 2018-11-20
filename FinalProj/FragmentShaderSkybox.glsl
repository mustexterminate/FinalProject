#version 450

//Texture Coordinates
in vec3 TexCoords;
//Output
out vec4 color;
uniform samplerCube texture;

void main()
{
	color = texture(texture, TexCoords);
}