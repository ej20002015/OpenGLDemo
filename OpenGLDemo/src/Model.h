#pragma once

#include "scene.h"

#include "Mesh.h"

#include <string>
#include <vector>

class Model
{
public:
	Model(const std::string& filePath);

	inline const std::vector<Mesh>& getMeshes() const { return meshes; }

private:
	std::vector<Mesh> meshes;
	std::vector<std::shared_ptr<Texture>> loadedTextures;
	std::string directory;

	void loadModel(const std::string filePath);
	void processNode(aiNode* node, const aiScene* scene);
	void processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<std::shared_ptr<Texture>> loadMaterialTextures(aiMaterial* material, aiTextureType aiType);
};