#include "ModelManager.h"

#include <Utility/SearchFile.h>
#include "ModelLoader.h"

#include <cassert>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

int ModelManager::LoadModel(std::string filePath) {
	// ファイルパスの確認と修正
	std::string fileName = FilePathChecker(filePath);

	// すでに読み込んでいたらIDを返す
	const auto it = modelFilePaths.find(filePath);
	if (it != modelFilePaths.end()) {
		return it->second;
	}

	//idの設定
	int id = nextID_++;
	modelFilePaths[filePath] = id;

	//Assimp
	Assimp::Importer importer;
	std::string path = (filePath + fileName);
	const aiScene* scene = nullptr;
	scene = importer.ReadFile(path.c_str(), aiProcess_MakeLeftHanded | aiProcess_FlipWindingOrder | aiProcess_FlipUVs | aiProcess_Triangulate);

	//読み込み
	if (scene->hasSkeletons()) {
		nodeModelDataMap[id] = WritingNodeModelData(scene, filePath);
	} else {
		skinningModelDataMap[id] = WritingSkinningModelData(scene, filePath);
	}

	return id;
}

std::string ModelManager::FilePathChecker(std::string& filePath) {
	//Assets/から始まっているか確認(Assets/Modelの可能性もあるのでAssets/のみ確認)
	std::string formatFirst = "Assets/";
	std::string factFilePath = "";
	if (filePath.length() < formatFirst.length()) {
		factFilePath = "Assets/Texture/" + filePath;
	} else {
		for (int i = 0; i < formatFirst.length(); ++i) {
			if (filePath[i] != formatFirst[i]) {
				factFilePath = "Assets/Texture/" + filePath;
				break;
			}

			if (i == formatFirst.length() - 1) {
				factFilePath = filePath;
			}
		}
	}
	filePath = factFilePath;

	auto objfile = SearchFiles(filePath, ".obj");

	//objが見つからなかったら
	if (objfile.size() != 1) {
		auto glbfile = SearchFiles(filePath, ".glb");

		//glbも見つからなかったら
		if (glbfile.size() != 1) {
			auto gltffile = SearchFiles(filePath, ".gltf");

			//gltfも見つからなかったら
			if (gltffile.size() != 1) {
				assert(false && "Can't find .obj or glb file");
				return "";
			}

			//gltfが見つかったら
			glbfile = gltffile;
		}

		//glbが見つかったら
		objfile = glbfile;
	}

	return objfile[0];
}

NodeModelData ModelManager::WritingNodeModelData(const aiScene* scene, std::string filePath) {
	NodeModelData result;
	//コードがごちゃつくのでModelLoaderに処理を投げる
	result.vertices = ModelLoader::LoadVertices(scene);
	result.indices = ModelLoader::LoadIndices(scene);
	result.materials = ModelLoader::LoadMaterials(scene, filePath, nullptr);
	result.materialIndex = ModelLoader::LoadMaterialIndices(scene);
	result.rootNode = ModelLoader::ReadNode(scene->mRootNode);

	return result;
}

SkinningModelData ModelManager::WritingSkinningModelData(const aiScene* scene, std::string filePath) {
	return SkinningModelData();
}
