#pragma once
#include "Item.h"
#include <Tool/Binary/BinaryManager.h>
#include <Assets/Model/ModelManager.h>

class ItemManager {
public:

	~ItemManager();

	void Initialize(SHEngine::ModelManager* modelManager);
	const Item& GetItem(std::wstring itemName) const;

	void DrawImGui();

private:

	void SaveModel();
	void LoadModel();

	void SaveItem();
	void LoadItem();

	void SaveBaseParam();
	void LoadBaseParam();

	//パラメータ一覧と基礎値
	std::vector<ItemParam> baseParam_;
	std::vector<Item> items_;

	std::map<int, std::string> modelIDtoName_;

	SHEngine::ModelManager* modelManager_ = nullptr;
	BinaryManager binaryManager_;

	static inline const std::string modelFile_ = "ModelIDMap.bin";
	static inline const std::string itemFile_ = "ItemData.bin";
	static inline const std::string baseParamFile_ = "BaseParamData.bin";
};
