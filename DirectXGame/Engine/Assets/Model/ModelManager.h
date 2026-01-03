#pragma once
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <assimp/scene.h>

#include "ModelData.h"

class ModelManager {
public:

	ModelManager() = default;
	~ModelManager() = default;

	void Initialize();

	int LoadModel(std::string filePath);

	//失敗したらLogとキューブが返る
	NodeModelData& GetNodeModelData(int id);
	//失敗したらLogとキューブが返る
	SkinningModelData& GetSkinningModelData(int id);

private:

	std::string FilePathChecker(std::string& filePath);

	NodeModelData WritingNodeModelData(const aiScene* scene, std::string filePath);
	SkinningModelData WritingSkinningModelData(const aiScene* scene, std::string filePath);

	std::unordered_map<std::string, int> modelFilePaths{};

	std::map<int, NodeModelData> nodeModelDataMap{};
	std::map<int, SkinningModelData> skinningModelDataMap{};

	int nextID_ = 0;

};
