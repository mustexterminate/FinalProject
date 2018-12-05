#version 450 //GLSL version your computer supports

in vec2 UV;
in vec3 normalWorld;
in vec3 vertexPositionWorld;
in vec3 TexCoords;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;

in vec3 LightDirection_tangentspace;
in vec3 EyeDirection_tangentspace;

out vec4 daColor;

uniform sampler2D myTextureSampler;
uniform sampler2D NormalTextureSampler;
uniform vec3 ambientLight;
uniform vec3 lightPositionWorld;
uniform vec3 eyePositionWorld;
uniform vec3 collideColour;
uniform float lightPowerSpec;
uniform float lightPowerDiff;
uniform mat4 V;
uniform mat4 M;
uniform mat3 MV3x3;

void main()
{
	vec3 materialAmbientColor = texture( myTextureSampler, UV ).rgb;
	vec4 materialDiffuseColor = vec4(texture( myTextureSampler, UV ).rgb, 1.0);
	vec4 materialSpecularColor = vec4(0.8, 0.3, 0.3, 1.0);
	vec3 LightColor = vec3(0.5,0.3,0.4);
	vec3 TextureNormal_tangentspace = normalize(texture( NormalTextureSampler, UV ).rgb*2.0 - 1.0);

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

	// Distance to the light
	float distance = length( LightPosition_worldspace - Position_worldspace );

	// Normal of the computed fragment, in camera space
	vec3 n = TextureNormal_tangentspace;
	// Direction of the light (from the fragment to the light)
	vec3 l = normalize(LightDirection_tangentspace);
	// Cosine of the angle between the normal and the light direction, 
	// clamped above 0
	//  - light is at the vertical of the triangle -> 1
	//  - light is perpendicular to the triangle -> 0
	//  - light is behind the triangle -> 0
	float cosTheta = clamp( dot( n,l ), 0,1 );

	// Eye vector (towards the camera)
	vec3 E = normalize(EyeDirection_tangentspace);
	// Direction in which the triangle reflects the light
	vec3 R = reflect(-l,n);
	// Cosine of the angle between the Eye vector and the Reflect vector,
	// clamped to 0
	//  - Looking into the reflection -> 1
	//  - Looking elsewhere -> < 1
	float cosAlpha = clamp( dot( E,R ), 0,1 );

	daColor = vec4(materialAmbientColor * ambientLight, 1.0) + vec4(collideColour, 1.0f)
	+ (materialDiffuseColor * clamp(diffuseLight, 0, 1) * vec4(LightColor, 1.0) * lightPowerDiff)
	+ (materialSpecularColor * specularLight * vec4(LightColor, 1.0) * lightPowerSpec);
}
