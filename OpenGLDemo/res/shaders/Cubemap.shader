#shader vertex
#version 330 core

layout(location = 0) in vec3 a_position;

uniform mat4 u_viewMatrix;
uniform mat4 u_projectionMatrix;

out vec3 texCoord;

void main()
{
    texCoord = a_position;
    vec4 position = u_projectionMatrix * u_viewMatrix * vec4(a_position, 1.0f);
    gl_Position = position.xyww;
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec3 texCoord;

uniform samplerCube u_cubemap;

void main()
{
    color = texture(u_cubemap, texCoord);
}