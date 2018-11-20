#version 330

//Texture Coordinates
in vec3 TexCoords;
//Output
out vec4 color;
uniform samplerCube cubemap;

void main()
{
	color = texture(cubemap, TexCoords);
	color = vec4(0.0f, 0.0f, 1.0f, 1.0f);
}