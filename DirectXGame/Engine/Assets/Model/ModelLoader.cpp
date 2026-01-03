#include "ModelLoader.h"

Node ModelLoader::ReadNode(const aiNode* node) {
	Node result;

	aiMatrix4x4 aiLocalMatrix = node->mTransformation;
	aiLocalMatrix.Transpose();
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			result.localMatrix.m[i][j] = aiLocalMatrix[i][j];

	result.name = node->mName.C_Str();// Node名を格納

	result.children.resize(node->mNumChildren); // 子供の数だけ確保
	for (uint32_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex) {
		//再帰的に読んで階層構造を作っていく
		result.children[childIndex] = ReadNode(node->mChildren[childIndex]);
	}

	return result;
}

std::vector<VertexData> ModelLoader::LoadVertices(const aiScene* scene) {
	std::vector<VertexData> vertices;

	for (uint32_t mesh = 0; mesh < scene->mNumMeshes; ++mesh) {
		aiMesh* ai_mesh = scene->mMeshes[mesh];
		for (uint32_t v = 0; v < ai_mesh->mNumVertices; ++v) {
			VertexData vertex{};
			//位置
			vertex.position.x = ai_mesh->mVertices[v].x;
			vertex.position.y = ai_mesh->mVertices[v].y;
			vertex.position.z = ai_mesh->mVertices[v].z;
			vertex.position.w = 1.0f;
			//法線
			if (ai_mesh->HasNormals()) {
				vertex.normal.x = ai_mesh->mNormals[v].x;
				vertex.normal.y = ai_mesh->mNormals[v].y;
				vertex.normal.z = ai_mesh->mNormals[v].z;
			}
			//UV
			if (ai_mesh->HasTextureCoords(0)) {
				vertex.texcoord.x = ai_mesh->mTextureCoords[0][v].x;
				vertex.texcoord.y = ai_mesh->mTextureCoords[0][v].y;
			}
			vertices.push_back(vertex);
		}
	}

	return vertices;
}

std::vector<uint32_t> ModelLoader::LoadIndices(const aiScene* scene) {
	std::vector<uint32_t> indices;

	for (uint32_t mesh = 0; mesh < scene->mNumMeshes; ++mesh) {
		aiMesh* ai_mesh = scene->mMeshes[mesh];

		for (uint32_t f = 0; f < ai_mesh->mNumFaces; ++f) {
			aiFace& face = ai_mesh->mFaces[f];

			for (uint32_t i = 0; i < face.mNumIndices; ++i) {
				indices.push_back(face.mIndices[i]);
			}
		}
	}

	return indices;
}

std::vector<Material> ModelLoader::LoadMaterials(const aiScene* scene, std::string directoryPath, TextureManager* textureManager) {
	std::vector<Material> materials;

	for (uint32_t i = 0; i < scene->mNumMaterials; ++i) {
		aiMaterial* ai_material = scene->mMaterials[i];
		Material material{};

		//テクスチャ読み込み
		aiString texturePath;
		if (ai_material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS) {
			std::string path = texturePath.C_Str();
			material.textureIndex = textureManager->LoadTexture(directoryPath + path);
		} else {
			material.textureIndex = 0; //テクスチャが無い場合はデフォルトテクスチャを使う
		}

		materials.push_back(material);
	}

	return materials;
}

std::vector<uint32_t> ModelLoader::LoadMaterialIndices(const aiScene* scene) {
	std::vector<uint32_t> result;

	for (uint32_t mesh = 0; mesh < scene->mNumMeshes; ++mesh) {
		aiMesh* ai_mesh = scene->mMeshes[mesh];

		for (uint32_t f = 0; f < ai_mesh->mNumFaces; ++f) {
			for (uint32_t i = 0; i < ai_mesh->mFaces[f].mNumIndices; ++i) {
				result.push_back(ai_mesh->mMaterialIndex);
			}
		}
	}

	return result;
}
