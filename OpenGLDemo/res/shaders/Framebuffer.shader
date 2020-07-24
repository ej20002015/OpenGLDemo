#shader vertex
#version 330 core

layout(location = 0) in vec2 a_position;
layout(location = 1) in vec2 a_texCoord;

out vec2 texCoord;

void main()
{
    gl_Position = vec4(a_position.x, a_position.y, 0.0, 1.0);
    texCoord = a_texCoord;
}

#shader fragment
#version 330 core

in vec2 texCoord;

layout(location = 0) out vec4 color;

uniform sampler2D u_framebufferTexture;

void main()
{
    color = texture(u_framebufferTexture, texCoord);
    /*float average = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
    color = vec4(average, average, average, color.z);*/
}