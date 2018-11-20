#version 330

//Texture Coordinates
in vec3 TexCoords;
//Output
out vec4 color;
uniform samplerCube texture;

void main()
{
	color = texture(texture, TexCoords);
	color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}