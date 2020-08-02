#shader vertex
#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texCoord;

uniform mat4 u_modelMatrix;
uniform mat4 u_viewMatrix;
uniform mat4 u_projectionMatrix;
uniform mat4 u_lightViewProjectionMatrix;

out vec3 normal;
out vec3 fragmentPosition;
out vec4 fragmentPositionLightSpace;
out vec2 texCoord;

void main()
{
	gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * vec4(a_position, 1.0f);
	normal = mat3(transpose(inverse(u_modelMatrix))) * a_normal;
	fragmentPosition = vec3(u_modelMatrix * vec4(a_position, 1.0f));
	fragmentPositionLightSpace = u_lightViewProjectionMatrix * vec4(fragmentPosition, 1.0);
	texCoord = a_texCoord;
};

#shader fragment
#version 330 core

struct Material
{
	sampler2D diffuse0;
	sampler2D specular0;
	float shininess;
};

struct DirectionalLight
{
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

vec3 calculateDirectionalLight(DirectionalLight directionalLight, vec3 normal, vec3 viewDirection);

struct PointLight
{
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constantTerm;
	float linearTerm;
	float quadraticTerm;
};

struct LightElements
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

LightElements calculatePointLight(PointLight pointLight, vec3 normal, vec3 viewDirection, vec3 fragmentPosition);

struct SpotLight
{
	vec3 position;
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constantTerm;
	float linearTerm;
	float quadraticTerm;

	float cutoff;
	float outerCutoff;
};

vec3 calculateSpotLight(SpotLight spotLight, vec3 normal, vec3 viewDirection, vec3 fragmentPosition);

vec3 calculateEnvironmentReflection(samplerCube environment, vec3 normalisedNormal, vec3 viewDirection);

vec3 calculateEnvironmentRefraction(samplerCube environment, vec3 normalisedNormal, vec3 viewDirection, float refractiveIndexRatio);

float calculateShadow(sampler2D shadowMap, vec4 fragmentPositionLightSpace, vec3 normalisedNormal, vec3 lightPosition, vec3 fragmentPosition);

in vec3 normal;
in vec3 fragmentPosition;
in vec4 fragmentPositionLightSpace;
in vec2 texCoord;

layout(location = 0) out vec4 colour;

uniform Material u_material;
uniform vec3 u_cameraPosition;

//uniform DirectionalLight u_directionalLight;
uniform PointLight u_pointLight;
//uniform SpotLight u_spotLight;

uniform samplerCube u_environment;
uniform float u_refractiveIndexRatio;

uniform sampler2D u_shadowMap;

const float eps = 0.0000005f;

void main()
{
	//Calculate properites to be used in all light calculations
	vec3 normalisedNormal = normalize(normal);
	vec3 viewDirection = normalize(u_cameraPosition - fragmentPosition);

	//Calculate contribution of each light
	//vec3 outputColour = calculateDirectionalLight(u_directionalLight, normalisedNormal, viewDirection);

	/*for (int i = 0; i < 4; ++i)
	{
		outputColour += calculatePointLight(u_pointLight[i], normalisedNormal, viewDirection, fragmentPosition);
	}*/

	LightElements outputColourPointLight = calculatePointLight(u_pointLight, normalisedNormal, viewDirection, fragmentPosition);

	//CALCULATE SHADOW
	float shadow = calculateShadow(u_shadowMap, fragmentPositionLightSpace, normalisedNormal, u_pointLight.position, fragmentPosition);

	vec3 outputColour = outputColourPointLight.ambient + ((1.0f - shadow) * outputColourPointLight.ambient) + ((1.0f - shadow) * outputColourPointLight.specular);

	//outputColour += calculateSpotLight(u_spotLight, normalisedNormal, viewDirection, fragmentPosition);

	//outputColour += calculateEnvironmentReflection(u_environment, normalisedNormal, viewDirection) * 0.05f;
	//outputColour = calculateEnvironmentRefraction(u_environment, normalisedNormal, viewDirection, u_refractiveIndexRatio);

	//Set colour accordingly
	colour = vec4(outputColour, 1.0f);
};

vec3 calculateDirectionalLight(DirectionalLight directionalLight, vec3 normal, vec3 viewDirection)
{
	vec3 ambient = vec3(0.0f);
	vec3 diffuse = vec3(0.0f);
	vec3 specular = vec3(0.0f);

	vec3 lightDirection = normalize(-directionalLight.direction);

	//Set up some ambient lighting
	ambient = directionalLight.ambient * vec3(texture(u_material.diffuse0, texCoord));

	//Set up diffuse lighting
	float diffusedValue = max(dot(normal, lightDirection), 0.0f);
	diffuse = directionalLight.diffuse * (diffusedValue * vec3(texture(u_material.diffuse0, texCoord)));

	//Set up specular lighting
	vec3 halfwayDirection = normalize(lightDirection + viewDirection);

	float specularValue = pow(max(dot(normal, halfwayDirection), 0.0f), u_material.shininess);
	specular = directionalLight.specular * (specularValue * vec3(texture(u_material.specular0, texCoord)));

	return ambient + diffuse + specular;
}

LightElements calculatePointLight(PointLight pointLight, vec3 normal, vec3 viewDirection, vec3 fragmentPosition)
{
	vec3 ambient = vec3(0.0f);
	vec3 diffuse = vec3(0.0f);
	vec3 specular = vec3(0.0f);

	vec3 lightDirection = normalize(pointLight.position - fragmentPosition);

	//Set up some ambient lighting
	ambient = pointLight.ambient * vec3(texture(u_material.diffuse0, texCoord));

	//Set up diffuse lighting
	float diffusedValue = max(dot(normal, lightDirection), 0.0f);
	diffuse = pointLight.diffuse * (diffusedValue * vec3(texture(u_material.diffuse0, texCoord)));

	//Set up specular lighting
	vec3 halfwayDirection = normalize(lightDirection + viewDirection);

	float specularValue = pow(max(dot(normal, halfwayDirection), 0.0f), u_material.shininess);
	specular = pointLight.specular * (specularValue * vec3(texture(u_material.specular0, texCoord)));


	//Calulate attenuation factor and adjust light values accordingly
	float distance = length(pointLight.position - fragmentPosition);
	float attenuation = 1.0f / (pointLight.constantTerm + (pointLight.linearTerm * distance) + (pointLight.quadraticTerm * distance * distance));

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	LightElements returnValue;
	returnValue.ambient = ambient;
	returnValue.diffuse = diffuse;
	returnValue.specular = specular;

	return returnValue;
}

vec3 calculateSpotLight(SpotLight spotLight, vec3 normal, vec3 viewDirection, vec3 fragmentPosition)
{
	vec3 ambient = vec3(0.0f);
	vec3 diffuse = vec3(0.0f);
	vec3 specular = vec3(0.0f);

	vec3 lightDirection = normalize(spotLight.position - fragmentPosition);

	//Set up some ambient lighting regardless of being in the spotlight
	ambient = spotLight.ambient * vec3(texture(u_material.diffuse0, texCoord));

	//Set up diffuse lighting
	float diffusedValue = max(dot(normal, lightDirection), 0.0f);
	diffuse = spotLight.diffuse * (diffusedValue * vec3(texture(u_material.diffuse0, texCoord)));

	//Set up specular lighting
	vec3 halfwayDirection = normalize(lightDirection + viewDirection);

	float specularValue = pow(max(dot(normal, halfwayDirection), 0.0f), u_material.shininess);
	specular = spotLight.specular * (specularValue * vec3(texture(u_material.specular0, texCoord)));

	//Calulate attenuation factor and adjust light values accordingly
	float distance = length(spotLight.position - fragmentPosition);
	float attenuation = 1.0f / (spotLight.constantTerm + (spotLight.linearTerm * distance) + (spotLight.quadraticTerm * distance * distance));

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	//Calculate specular and diffuse lighting if fragment is within the spotlight's radius
	float theta = dot(lightDirection, normalize(-spotLight.direction));
	float epsilon = spotLight.cutoff - spotLight.outerCutoff;
	float intensity = clamp((theta - spotLight.outerCutoff) / epsilon, 0.0f, 1.0f);

	diffuse *= intensity;
	specular *= intensity;

	return ambient + diffuse + specular;
}

vec3 calculateEnvironmentReflection(samplerCube environment, vec3 normalisedNormal, vec3 viewDirection)
{
	vec3 reflectedVector = reflect(-viewDirection, normalisedNormal);
	vec3 reflectedColour = texture(environment, reflectedVector).rgb;
	//TODO: change from using specular map to using a seperate reflection map
	//return vec3(texture(u_material.specular0, texCoord)) * reflectedColour;
	return reflectedColour;
}

vec3 calculateEnvironmentRefraction(samplerCube environment, vec3 normalisedNormal, vec3 viewDirection, float refractiveIndexRatio)
{
	vec3 refractedVector = refract(-viewDirection, normalisedNormal, refractiveIndexRatio);
	vec3 refractedColour = texture(environment, refractedVector).rgb;
	return refractedColour;
}

float calculateShadow(sampler2D shadowMap, vec4 fragmentPositionLightSpace, vec3 normalisedNormal, vec3 lightPosition, vec3 fragmentPosition)
{
	if (fragmentPositionLightSpace.w < 1.0f)
		return 0.0f;

	vec3 projectedCoordinates = fragmentPositionLightSpace.xyz / fragmentPositionLightSpace.w;
	projectedCoordinates = projectedCoordinates * 0.5f + 0.5f;

	float closestDepth = texture(shadowMap, projectedCoordinates.xy).r;
	float currentDepth = projectedCoordinates.z;

	vec3 lightDirection = normalize(lightPosition - fragmentPosition);
	float bias = max(0.000005f * (1.0f - dot(normalisedNormal, lightDirection)), 0.0000005f);
	float shadow = currentDepth - bias > closestDepth ? 1.0f : 0.0f;

	return shadow;
}