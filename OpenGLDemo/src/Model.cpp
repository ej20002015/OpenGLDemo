#include "Model.h"

#include "Importer.hpp"
#include "postprocess.h"

#include <algorithm>

Model::Model(const std::string& filePath)
    : directory(filePath.substr(0, filePath.find_last_of('/')))
{
    loadModel(filePath);
}

void Model::loadModel(const std::string filePath)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs);

    //If an error occurs when loading the model then stop. Else, start recursively processing the nodes in the model that has been loaded
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !(scene->mRootNode))
        std::cerr << "ERROR: Problem occured when loading model " << filePath << "- " << importer.GetErrorString() << std::endl;
    else
        processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
    //Process each mesh of the node
    for (uint32_t i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        processMesh(mesh, scene);
    }

    //Recursively process each child of this node
    for (uint32_t i = 0; i < node->mNumChildren; ++i)
        processNode(node->mChildren[i], scene);
}

void Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    //PROCESS VERTEX DATA

    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 textureCoordinates;
    };

    std::vector<Vertex> vertices;

    //Process all the vertex data for the mesh
    for (uint32_t i = 0; i < mesh->mNumVertices; ++i)
    {
        Vertex vertex;

        //Retrieve vertex position data
        glm::vec3 position;
        position.x = mesh->mVertices[i].x;
        position.y = mesh->mVertices[i].y;
        position.z = mesh->mVertices[i].z;

        vertex.position = position;

        //Retrieve vertex normal data
        glm::vec3 normal;
        normal.x = mesh->mNormals[i].x;
        normal.y = mesh->mNormals[i].y;
        normal.z = mesh->mNormals[i].z;

        vertex.normal = normal;

        //Retrieve vertex texture coordinates if they exist
        if (mesh->mTextureCoords[0])
        {
            glm::vec2 textureCoordinates;
            textureCoordinates.x = mesh->mTextureCoords[0][i].x;
            textureCoordinates.y = mesh->mTextureCoords[0][i].y;

            vertex.textureCoordinates = textureCoordinates;
        }
        else
            vertex.textureCoordinates = { 0.0f, 0.0f };

        vertices.push_back(vertex);
    }

    //Specify buffer layour
    VertexBufferLayout vertexBufferLayout;
    vertexBufferLayout.push<float>(3);
    vertexBufferLayout.push<float>(3);
    vertexBufferLayout.push<float>(2);

    //Create vertex buffer with data
    std::shared_ptr<VertexBuffer> vertexBuffer;
    if (vertices.size() > 0)
        vertexBuffer = std::make_shared<VertexBuffer>(&vertices[0], vertices.size() * vertexBufferLayout.getStride());
    else
        return;

    //Create vertex array with the buffer and the layout
    auto vertexArray = std::make_shared<VertexArray>();
    vertexArray->addBuffer(vertexBuffer, vertexBufferLayout);

    //PROCESS INDEX DATA
    std::vector<uint32_t> indices;

    //Process the index data by inspecting each 'face' of the mesh
    for (uint32_t i = 0; i < mesh->mNumFaces; ++i)
    {
        aiFace face = mesh->mFaces[i];
        for (uint32_t j = 0; j < face.mNumIndices; ++j)
            indices.push_back(face.mIndices[j]);
    }

    //Create index buffer with index data
    std::shared_ptr<IndexBuffer> indexBuffer;
    if (indices.size() > 0)
        indexBuffer = std::make_shared<IndexBuffer>(&indices[0], indices.size());
    else
        return;

    //PROCESS MATERIALS
    std::vector<std::shared_ptr<Texture>> textures;

    //If materials exist for the mesh
    if (mesh->mMaterialIndex > 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        //Process all the diffuse maps of the material
        std::vector<std::shared_ptr<Texture>> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE);
        textures.insert(textures.begin(), diffuseMaps.begin(), diffuseMaps.end());

        //Process all the specular maps of the material
        std::vector<std::shared_ptr<Texture>> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR);
        textures.insert(textures.begin(), specularMaps.begin(), specularMaps.end());
    }

    meshes.push_back(Mesh(vertexArray, indexBuffer, textures));
}

std::vector <std::shared_ptr<Texture>> Model::loadMaterialTextures(aiMaterial* material, aiTextureType aiType)
{
    std::vector<std::shared_ptr<Texture>> textures;

    //Proccess all textures of the specified type in the material and construct a texture object for each one
    for (uint32_t i = 0; i < material->GetTextureCount(aiType); ++i)
    {
        bool skip = false;
        aiString textureName;
        material->GetTexture(aiType, i, &textureName);

        //If texture has already been loaded then don't do it again
        for (auto texture : loadedTextures)
        {
            std::string filePath = directory + "/" + textureName.C_Str();
            if (!std::strcmp(texture->getFilePath().c_str(), filePath.c_str()))
            {
                textures.push_back(texture);
                skip = true;
            }
        }

        if (!skip)
        {
            Texture::Type type;
            switch (aiType)
            {
            case aiTextureType_DIFFUSE:
                type = Texture::Type::DIFFUSE;
                break;
            case aiTextureType_SPECULAR:
                type = Texture::Type::SPECULAR;
                break;
            default:
                type = Texture::Type::OTHER;
                break;
            }

            auto texture = std::make_shared<Texture>(directory + "/" + textureName.C_Str(), type);
            loadedTextures.push_back(texture);
            textures.push_back(texture);
        }
    }

    return textures;
}
