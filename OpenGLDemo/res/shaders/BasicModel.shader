#shader vertex
#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texCoord;

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
	fragmentPosition = vec3(u_modelMatrix * vec4(a_position, 1.0f));
	texCoord = a_texCoord;
};

#shader fragment
#version 330 core

in vec2 texCoord;

layout(location = 0) out vec4 color;

struct Material
{
	sampler2D diffuse0;
};

uniform Material u_material;

void main()
{
	color = texture(u_material.diffuse0, texCoord);
};