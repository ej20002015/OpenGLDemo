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

void Renderer::draw(const Model& model, Program& program) const
{
    for (auto mesh : model.getMeshes())
        draw(mesh, program);
}

void Renderer::draw(const Mesh& mesh, Program& program) const
{
    auto vertexArray = mesh.getVertexArray();
    auto indexBuffer = mesh.getIndexBuffer();
    auto textures = mesh.getTextures();

    vertexArray->bind();
    indexBuffer->bind();
    program.bind();

    uint32_t diffuseNum = 0, specularNum = 0;

    //Set texture uniforms and bind textures
    for (uint32_t i = 0; i < textures.size(); ++i)
    {
        auto texture = textures[i];
        texture->bind(i);
        Texture::Type type = texture->getType();

        switch (type)
        {
        case Texture::Type::DIFFUSE:
            program.setUniform1i("u_material.diffuse" + std::to_string(diffuseNum++), i);
            break;
        case Texture::Type::SPECULAR:
            program.setUniform1i("u_material.specular" + std::to_string(specularNum++), i);
            break;
        case Texture::Type::OTHER:
            break;
        default:
            break;
        }
    }

    glDrawElements(GL_TRIANGLES, indexBuffer->getCount(), GL_UNSIGNED_INT, nullptr);
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