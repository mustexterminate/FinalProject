#version 450 //GLSL version your computer supports

in vec2 UV;
in vec3 normalWorld;
in vec3 vertexPositionWorld;
in vec3 TexCoords;

out vec4 daColor;

uniform sampler2D myTextureSampler;
uniform samplerCube skybox;
uniform vec3 ambientLight;
uniform vec3 lightPositionWorld;
uniform vec3 eyePositionWorld;
uniform float lightPowerSpec;
uniform float lightPowerDiff;

void main()
{
	vec3 materialAmbientColor = texture( myTextureSampler, UV ).rgb;
	vec4 materialDiffuseColor = vec4(texture( myTextureSampler, UV ).rgb, 1.0);
	vec4 materialSpecularColor = vec4(0.5, 0.5, 0.5, 1.0);
	vec3 LightColor = vec3(0.5,0.5,0.5);

	// specular
	vec3 reflectedLightVectorWorld = reflect(-lightPositionWorld, normalWorld);
	vec3 eyeVectorWorld = normalize(eyePositionWorld - vertexPositionWorld);
	float s = clamp(dot(reflectedLightVectorWorld, eyeVectorWorld), 0, 1);
	s = pow(s,50);
	vec4 specularLight = vec4(s,0,0,1);

	// Diffuse
	vec3 lightVectorWorld = normalize(lightPositionWorld - vertexPositionWorld);
	float brightness = dot(lightVectorWorld, normalize(normalWorld));
	vec4 diffuseLight = vec4(brightness, brightness, brightness, 1.0);

	daColor = vec4(materialAmbientColor * ambientLight, 1.0) + texture(skybox, TexCoords) 
	+ (materialDiffuseColor * clamp(diffuseLight, 0, 1) * vec4(LightColor, 1.0) * lightPowerDiff) 
	+ (materialSpecularColor * specularLight * vec4(LightColor, 1.0) * lightPowerSpec);
}
