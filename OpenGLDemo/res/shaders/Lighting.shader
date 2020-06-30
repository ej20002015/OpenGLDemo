#shader vertex
#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 2) in vec3 a_normal;

uniform mat4 u_modelMatrix;
uniform mat4 u_viewMatrix;
uniform mat4 u_projectionMatrix;

out vec3 normal;
out vec3 fragmentPosition;

void main()
{
	gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * vec4(a_position, 1.0f);
	normal = mat3(transpose(inverse(u_modelMatrix))) * a_normal;
	fragmentPosition = vec3(u_viewMatrix * vec4(a_position, 1.0f));
};

#shader fragment
#version 330 core

in vec3 normal;
in vec3 fragmentPosition;

layout(location = 0) out vec4 colour;

uniform vec4 u_objectColour;
uniform vec4 u_lightColour;
uniform vec3 u_lightPosition;
uniform vec3 u_cameraPosition;

void main()
{
	//Set up some ambient lighting
	float ambientLightStrength = 0.1f;
	vec4 ambient = ambientLightStrength * u_lightColour;
	//Set the alpha channel back to no transparency
	ambient.a = 1.0f;

	//Set up diffuse lighting
	vec3 normalizedNormal = normalize(normal);
	vec3 lightDirection = normalize(u_lightPosition - fragmentPosition);

	float diffusedValue = max(dot(normalizedNormal, lightDirection), 0.0f);
	vec4 diffuse = diffusedValue * u_lightColour;
	diffuse.a = 1.0f;

	//Set up specular lighting
	float specularStrength = 0.5f;
	vec3 normalizedCameraDirection = normalize(u_cameraPosition - fragmentPosition);
	vec3 reflectedLightDirection = reflect(-lightDirection, normalizedNormal);

	float specularValue = pow(max(dot(normalizedCameraDirection, reflectedLightDirection), 0.0), 32);
	vec4 specular = specularStrength * specularValue * u_lightColour;
	specular.a = 1.0f;

	//Calculate final colour value
	colour = u_objectColour * (ambient + diffuse + specular);
};