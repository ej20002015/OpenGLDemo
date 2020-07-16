#shader vertex
#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_texCoord;
layout(location = 2) in vec3 a_normal;

uniform mat4 u_modelMatrix;
uniform mat4 u_viewMatrix;
uniform mat4 u_projectionMatrix;

out vec3 normal;
out vec3 fragmentPosition;
out vec2 texCoord;

void main()
{
	gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * vec4(a_position, 1.0f);
	normal = mat3(transpose(inverse(u_modelMatrix))) * a_normal;
	fragmentPosition = vec3(u_viewMatrix * vec4(a_position, 1.0f));
	texCoord = a_texCoord;
};

#shader fragment
#version 330 core

struct Material
{
	sampler2D diffuse;
	sampler2D specular;
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

vec3 calculatePointLight(PointLight pointLight, vec3 normal, vec3 viewDirection, vec3 fragmentPosition);

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

in vec3 normal;
in vec3 fragmentPosition;
in vec2 texCoord;

layout(location = 0) out vec4 colour;

uniform Material u_material;
uniform vec3 u_cameraPosition;

uniform DirectionalLight u_directionalLight;
uniform PointLight u_pointLight[4];
uniform SpotLight u_spotLight;

const float eps = 0.0000005f;

void main()
{
	//Calculate properites to be used in all light calculations
	vec3 normalisedNormal = normalize(normal);
	vec3 viewDirection = normalize(u_cameraPosition - fragmentPosition);

	//Calculate contribution of each light

	vec3 output = vec3(0.0f);

	//vec3 output = calculateDirectionalLight(u_directionalLight, normalisedNormal, viewDirection);

	for (int i = 0; i < 4; ++i)
	{
		//output += calculatePointLight(u_pointLight[i], normalisedNormal, viewDirection, fragmentPosition);
	}

	output += calculateSpotLight(u_spotLight, normalisedNormal, viewDirection, fragmentPosition);

	//Set colour accordingly
	colour = vec4(output, 1.0f);
};

vec3 calculateDirectionalLight(DirectionalLight directionalLight, vec3 normal, vec3 viewDirection)
{
	vec3 ambient = vec3(0.0f);
	vec3 diffuse = vec3(0.0f);
	vec3 specular = vec3(0.0f);

	vec3 lightDirection = normalize(-directionalLight.direction);

	//Set up some ambient lighting
	ambient = directionalLight.ambient * vec3(texture(u_material.diffuse, texCoord));

	//Set up diffuse lighting
	float diffusedValue = max(dot(normal, lightDirection), 0.0f);
	diffuse = directionalLight.diffuse * (diffusedValue * vec3(texture(u_material.diffuse, texCoord)));

	//Set up specular lighting
	vec3 reflectedLightDirection = reflect(-lightDirection, normal);

	float specularValue = pow(max(dot(viewDirection, reflectedLightDirection), 0.0f), u_material.shininess);
	specular = directionalLight.specular * (specularValue * vec3(texture(u_material.specular, texCoord)));

	return ambient + diffuse + specular;
}

vec3 calculatePointLight(PointLight pointLight, vec3 normal, vec3 viewDirection, vec3 fragmentPosition)
{
	vec3 ambient = vec3(0.0f);
	vec3 diffuse = vec3(0.0f);
	vec3 specular = vec3(0.0f);

	vec3 lightDirection = normalize(pointLight.position - fragmentPosition);

	//Set up some ambient lighting
	ambient = pointLight.ambient * vec3(texture(u_material.diffuse, texCoord));

	//Set up diffuse lighting
	float diffusedValue = max(dot(normal, lightDirection), 0.0f);
	diffuse = pointLight.diffuse * (diffusedValue * vec3(texture(u_material.diffuse, texCoord)));

	//Set up specular lighting
	vec3 reflectedLightDirection = reflect(-lightDirection, normal);

	float specularValue = pow(max(dot(viewDirection, reflectedLightDirection), 0.0f), u_material.shininess);
	specular = pointLight.specular * (specularValue * vec3(texture(u_material.specular, texCoord)));


	//Calulate attenuation factor and adjust light values accordingly
	float distance = length(pointLight.position - fragmentPosition);
	float attenuation = 1.0f / (pointLight.constantTerm + (pointLight.linearTerm * distance) + (pointLight.quadraticTerm * distance * distance));

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return ambient + diffuse + specular;
}

vec3 calculateSpotLight(SpotLight spotLight, vec3 normal, vec3 viewDirection, vec3 fragmentPosition)
{
	vec3 ambient = vec3(0.0f);
	vec3 diffuse = vec3(0.0f);
	vec3 specular = vec3(0.0f);

	vec3 lightDirection = normalize(spotLight.position - fragmentPosition);

	//Set up some ambient lighting regardless of being in the spotlight
	ambient = spotLight.ambient * vec3(texture(u_material.diffuse, texCoord));
	
	//Set up diffuse lighting
	float diffusedValue = max(dot(normal, lightDirection), 0.0f);
	diffuse = spotLight.diffuse * (diffusedValue * vec3(texture(u_material.diffuse, texCoord)));

	//Set up specular lighting
	vec3 reflectedLightDirection = reflect(-lightDirection, normal);

	float specularValue = pow(max(dot(viewDirection, reflectedLightDirection), 0.0f), u_material.shininess);
	specular = spotLight.specular * (specularValue * vec3(texture(u_material.specular, texCoord)));

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