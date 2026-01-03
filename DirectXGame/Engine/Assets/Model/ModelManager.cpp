#include "ModelManager.h"

#include <Utility/SearchFile.h>
#include "ModelLoader.h"

#include <cassert>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

namespace {
	std::string ExtensionSearcher(std::string directoryPath, std::vector<std::string> ext) {
		auto files = SearchFiles(directoryPath, ext.front());
		if (files.size() > 0) {
			return files.front();
		}

		ext.erase(ext.begin());
		return ExtensionSearcher(directoryPath, ext);
	}
}

void ModelManager::Initialize(TextureManager* textureManager, DrawDataManager* drawDataManager) {
	modelFilePaths.clear();
	nodeModelDataMap.clear();
	skinningModelDataMap.clear();
	textureManager_ = textureManager;
	drawDataManager_ = drawDataManager;
	nextID_ = 0;

	logger_ = getLogger("Engine");

	LoadModel("Assets/.EngineResource/Model/cube");
	LoadModel("Assets/.EngineResource/Model/DefaultDesc");
}

int ModelManager::LoadModel(std::string filePath) {
	// ファイルパスの確認と修正
	std::string fileName = FilePathChecker(filePath);

	// すでに読み込んでいたらIDを返す
	const auto it = modelFilePaths.find(filePath);
	if (it != modelFilePaths.end()) {
		logger_->debug("Model already loaded: {}", filePath);
		return it->second;
	}

	logger_->info("Loading Model: {}", filePath + fileName);

	//idの設定
	int id = nextID_++;
	modelFilePaths[filePath] = id;

	//Assimp
	Assimp::Importer importer;
	std::string path = (filePath + "/" + fileName);
	const aiScene* scene = nullptr;
	scene = importer.ReadFile(path.c_str(), aiProcess_MakeLeftHanded | aiProcess_FlipWindingOrder | aiProcess_FlipUVs | aiProcess_Triangulate);
	assert(scene && "ModelManager::LoadModel: Failed to load model");

	//読み込み
	if (!scene->hasSkeletons()) {
		NodeModelData result = WritingNodeModelData(scene, filePath);

		nodeModelDataMap[id] = result;

	} else {
		skinningModelDataMap[id] = WritingSkinningModelData(scene, filePath);
	}

	return id;
}

Animation ModelManager::LoadAnimation(std::string filePath) {

	FilePathChecker(filePath);


	return Animation();
}

NodeModelData& ModelManager::GetNodeModelData(int id) {
	auto it = nodeModelDataMap.find(id);
	if (it != nodeModelDataMap.end()) {
		return it->second;
	} else {
		logger_->error("Model ID not found: {}", id);
		assert(false && "ModelManager::GetNodeModelData: Model ID not found");
		return nodeModelDataMap[0]; //キューブをセット
	}
}

SkinningModelData& ModelManager::GetSkinningModelData(int id) {
	auto it = skinningModelDataMap.find(id);
	if (it != skinningModelDataMap.end()) {
		return it->second;
	} else {
		logger_->error("Model ID not found: {}", id);
		assert(false && "ModelManager::GetSkinningModelData: Model ID not found");
		return skinningModelDataMap[0]; //キューブをセット
	}
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

	std::vector<std::string> extensions = { ".fbx", ".obj", ".gltf", ".glb" };
	std::string fileName = ExtensionSearcher(filePath, extensions);

	return fileName;
}

NodeModelData ModelManager::WritingNodeModelData(const aiScene* scene, std::string filePath) {
	NodeModelData result;
	//コードがごちゃつくのでModelLoaderに処理を投げる
	result.vertices = ModelLoader::LoadVertices(scene);
	result.indices = ModelLoader::LoadIndices(scene);
	result.materials = ModelLoader::LoadMaterials(scene, filePath, textureManager_);
	result.materialIndex = ModelLoader::LoadMaterialIndices(scene);
	result.rootNode = ModelLoader::ReadNode(scene->mRootNode);

	//読み込めているかの確認
	bool isCorrect = true;
	if (result.vertices.empty()) {
		logger_->warn("Vertex is Empty!");
		isCorrect = false;
	}
	if (result.indices.empty()) {
		logger_->warn("Index is Empty!");
		isCorrect = false;
	}
	if (result.materials.empty()) {
		logger_->warn("Material is Empty!");
		isCorrect = false;
	}
	if (result.materialIndex.empty()) {
		logger_->warn("MaterialIndex is Empty!");
		isCorrect = false;
	}

	if (!isCorrect) {
		logger_->error("Failed to Load Model, So setting Cube instead.");
		assert(false && "ModelManager::LoadModel: Failed to load model");
		result = nodeModelDataMap[0]; //キューブをセット
	} else {
		//DrawDataの作成
		drawDataManager_->AddVertexBuffer(result.vertices);
		drawDataManager_->AddIndexBuffer(result.indices);
		int drawDataIndex = drawDataManager_->CreateDrawData();
		result.drawDataIndex = drawDataIndex;
	}

	return result;
}

SkinningModelData ModelManager::WritingSkinningModelData(const aiScene* scene, std::string filePath) {
	return SkinningModelData();
}
