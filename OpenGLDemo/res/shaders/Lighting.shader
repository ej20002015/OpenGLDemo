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

struct Light
{
	vec3 position;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
};

in vec3 normal;
in vec3 fragmentPosition;
in vec2 texCoord;

layout(location = 0) out vec4 colour;

uniform Material u_material;
uniform Light u_light;
uniform vec3 u_cameraPosition;

void main()
{
	//Set up some ambient lighting
	float ambientLightStrength = 0.1f;
	vec4 ambient = u_light.ambient * texture(u_material.diffuse, texCoord);
	//Set the alpha channel back to no transparency
	ambient.a = 1.0f;

	//Set up diffuse lighting
	vec3 normalizedNormal = normalize(normal);
	vec3 lightDirection = normalize(u_light.position - fragmentPosition);

	float diffusedValue = max(dot(normalizedNormal, lightDirection), 0.0f);
	vec4 diffuse = u_light.diffuse * (diffusedValue * texture(u_material.diffuse, texCoord));
	diffuse.a = 1.0f;

	//Set up specular lighting
	vec3 normalizedCameraDirection = normalize(u_cameraPosition - fragmentPosition);
	vec3 reflectedLightDirection = reflect(-lightDirection, normalizedNormal);

	float specularValue = pow(max(dot(normalizedCameraDirection, reflectedLightDirection), 0.0), u_material.shininess);
	vec4 specular = u_light.specular * (specularValue * texture(u_material.specular, texCoord));
	specular.a = 1.0f;

	//Calculate final colour value
	colour = ambient + diffuse + specular;
};