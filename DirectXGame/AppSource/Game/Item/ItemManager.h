#pragma once
#include "Item.h"
#include <Tool/Binary/BinaryManager.h>
#include <Assets/Model/ModelManager.h>

enum class ParamType : int
{
	HP,
	MP,
	Attack,
	Defense,
	Speed,
	Count,
};

inline const char* const* GetParamTypeNames()
{
	static const char* names[] = {
		"HP",
		"MP",
		"Attack",
		"Defense",
		"Speed",
	};
	return names;
}

inline int GetParamTypeCount()
{
	return static_cast<int>(ParamType::Count);
}


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

	std::vector<std::string> itemNameUtf8_;
	std::vector<const char*> itemNameCStr_;
	void UpdateItemNameCache();

	//パラメータ一覧と基礎値
	std::unordered_map<std::string, float> baseParam_;
	std::vector<Item> items_;

	std::map<int, std::string> modelIDtoName_;

	SHEngine::ModelManager* modelManager_ = nullptr;
	BinaryManager binaryManager_;

	static inline const std::string modelFile_ = "ModelIDMap.bin";
	static inline const std::string itemFile_ = "ItemData.bin";
	static inline const std::string baseParamFile_ = "BaseParamData.bin";
};
