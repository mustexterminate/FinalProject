#version 450 //GLSL version your computer supports

in vec2 UV;
in vec3 normalWorld;
in vec3 vertexPositionWorld;
in vec3 TexCoords;

out vec4 daColor;

uniform sampler2D myTextureSampler;
uniform vec3 ambientLight;
uniform vec3 lightPositionWorld;
uniform vec3 lightPositionWorld2;
uniform vec3 eyePositionWorld;
uniform vec3 collideColour;
uniform float lightPowerSpec;
uniform float lightPowerDiff;
uniform float lightPowerSpec2;
uniform float lightPowerDiff2;

void main()
{
	//First Light Position
	vec3 materialAmbientColor = texture( myTextureSampler, UV ).rgb;
	vec4 materialDiffuseColor = vec4(texture( myTextureSampler, UV ).rgb, 1.0);
	vec4 materialSpecularColor = vec4(0.8, 0.3, 0.3, 1.0);
	vec3 LightColor = vec3(0.3,0.5,0.4);

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

	vec4 EndColour1 = vec4(materialAmbientColor * ambientLight, 1.0)
	+ (materialDiffuseColor * clamp(diffuseLight, 0, 1) * vec4(LightColor, 1.0) * lightPowerDiff)
	+ (materialSpecularColor * specularLight * vec4(LightColor, 1.0) * lightPowerSpec);


	//Second Light Position
	// specular
	vec3 reflectedLightVectorWorld2 = reflect(-lightPositionWorld2, normalWorld);
	float s2 = clamp(dot(reflectedLightVectorWorld2, eyeVectorWorld), 0, 1);
	s2 = pow(s2,50);
	vec4 specularLight2 = vec4(s2,0,0,1);

	// Diffuse
	vec3 lightVectorWorld2 = normalize(lightPositionWorld2 - vertexPositionWorld);
	float brightness2 = dot(lightVectorWorld2, normalize(normalWorld));
	vec4 diffuseLight2 = vec4(brightness2, brightness2, brightness2, 1.0);

	vec4 EndColour2 = vec4(materialAmbientColor * ambientLight, 1.0)
	+ (materialDiffuseColor * clamp(diffuseLight2, 0, 1) * vec4(LightColor, 1.0) * lightPowerDiff2)
	+ (materialSpecularColor * specularLight2 * vec4(LightColor, 1.0) * lightPowerSpec2);

	daColor = (EndColour1 + EndColour2) / 1.3f + vec4(collideColour,1.0f);
}

