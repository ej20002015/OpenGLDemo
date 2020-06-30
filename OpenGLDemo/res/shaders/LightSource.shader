#shader vertex
#version 330 core

layout(location = 0) in vec4 a_position;

uniform mat4 u_modelMatrix;
uniform mat4 u_viewMatrix;
uniform mat4 u_projectionMatrix;

void main()
{
	gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * a_position;
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 colour;

void main()
{
	colour = vec4(1.0f);
};