#pragma once
#include "ModelData.h"
#include <assimp/scene.h>
#include <Assets/Texture/TextureManager.h>

namespace ModelLoader {
	Node ReadNode(const aiNode* node);

	std::vector<VertexData> LoadVertices(const aiScene* scene);

	std::vector<uint32_t> LoadIndices(const aiScene* scene);

	std::vector<Material> LoadMaterials(const aiScene* scene, std::string directoryPath, TextureManager* textureManager);
	std::vector<uint32_t> LoadMaterialIndices(const aiScene* scene);
}
