#shader vertex
#version 330 core

layout(location = 0) in vec3 a_position;

uniform mat4 u_modelMatrix;
uniform mat4 u_lightViewProjectionMatrix;

void main()
{
	gl_Position = u_lightViewProjectionMatrix * u_modelMatrix * vec4(a_position, 1.0f);
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 colour;

void main()
{
	colour = vec4(0.0f, 0.0f, 0.0f, 1.0f);
}