#include "Renderer.h"

#include <iostream>

void GLAPIENTRY debugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
    GLsizei length, const GLchar* message, const void* userParam)
{
    std::cerr << "GL CALLBACK: " << (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "")
        << "type = " << std::hex << type << ", severity = " << severity << ", message = "
        << message << std::endl;
    //std::cin.get();
    //exit(1);
}

void Renderer::draw(const VertexArray& vertexArray, const IndexBuffer& indexBuffer, const Program& program) const
{
    vertexArray.bind();
    indexBuffer.bind();
    program.bind();

    glDrawElements(GL_TRIANGLES, indexBuffer.getCount(), GL_UNSIGNED_INT, nullptr);
}

void Renderer::clear() const
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::initDebug()
{
    //Enable debug output
    #ifdef _DEBUG
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(debugMessageCallback, 0);
    #endif
}