#version 450 //GLSL version your computer supports

in vec2 UV;
//in vec3 normalWorld;
in vec3 vertexPositionWorld;
in vec3 TexCoords;
in mat3 TBN;

out vec4 daColor;

uniform sampler2D myTextureSampler;
uniform sampler2D normal_texture;
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
	// obtain normal from normal map in range [0,1]
	//normal = (2*color)-1
    vec3 normal = texture(normal_texture, UV).rgb;
    // transform normal vector to range [-1,1]
    normal = normalize(TBN * (normal * 2.0f - 1.0f));

	//First Light Position
	vec3 materialAmbientColor = texture( myTextureSampler, UV ).rgb;
	vec4 materialDiffuseColor = vec4(texture( myTextureSampler, UV ).rgb, 1.0);
	vec4 materialSpecularColor = vec4(0.8, 0.3, 0.3, 1.0);
	vec3 LightColor = vec3(0.3,0.5,0.1);

	// specular
	vec3 reflectedLightVectorWorld = TBN * reflect(-lightPositionWorld, normal);
	vec3 eyeVectorWorld = TBN * normalize(eyePositionWorld - vertexPositionWorld);
	float s = clamp(dot(reflectedLightVectorWorld, eyeVectorWorld), 0, 1);
	s = pow(s,50);
	vec4 specularLight = vec4(s,0,0,1);

	// Diffuse
	vec3 lightVectorWorld = TBN * normalize(lightPositionWorld - vertexPositionWorld);
	float brightness = dot(lightVectorWorld, normalize(normal));
	vec4 diffuseLight = vec4(brightness, brightness, brightness, 1.0);

	vec4 EndColour1 = vec4(materialAmbientColor * ambientLight, 1.0)
	+ (materialDiffuseColor * clamp(diffuseLight, 0, 1) * vec4(LightColor, 1.0) * lightPowerDiff)
	+ (materialSpecularColor * specularLight * vec4(LightColor, 1.0) * lightPowerSpec);


	//Second Light Position
	// specular
	vec3 reflectedLightVectorWorld2 = TBN * reflect(-lightPositionWorld2, normal);
	float s2 = clamp(dot(reflectedLightVectorWorld2, eyeVectorWorld), 0, 1);
	s2 = pow(s2,50);
	vec4 specularLight2 = vec4(s2,0,0,1);

	// Diffuse
	vec3 lightVectorWorld2 = TBN * normalize(lightPositionWorld2 - vertexPositionWorld);
	float brightness2 = dot(lightVectorWorld2, normalize(normal));
	vec4 diffuseLight2 = vec4(brightness2, brightness2, brightness2, 1.0);

	vec4 EndColour2 = vec4(materialAmbientColor * ambientLight, 1.0)
	+ (materialDiffuseColor * clamp(diffuseLight2, 0, 1) * vec4(LightColor, 1.0) * lightPowerDiff2)
	+ (materialSpecularColor * specularLight2 * vec4(LightColor, 1.0) * lightPowerSpec2);

	daColor = (EndColour1 + EndColour2) / 1.3f + vec4(collideColour,1.0f);
}

