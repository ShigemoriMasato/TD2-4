#pragma once
#include "Item.h"
#include <Tool/Binary/BinaryManager.h>
#include <Assets/Model/ModelManager.h>
#include <Tool/Json/JsonManager.h>
#include <memory>

enum class ParamType : int
{
	HP,
	MP,
	Attack,
	Defense,
	Speed,
	Count,
};

class ItemManager {
public:

	ItemManager();
	~ItemManager();
	void Initialize(SHEngine::ModelManager* modelManager);
	void DrawImGui();

	// アイテムの名前からアイテムデータを取得(名前リストがどこかにある想定)
	const Item& GetItem(std::wstring itemName) const;
	const Item& GetItem(int index);
	std::unordered_map<int, Item> GetAllItems() const { return items_; }

	// 描画等で必要になったときに ID を解決する
	int ResolveModelID(Item& item);

#ifdef USE_IMGUI 

	/// Editer用関数
	std::unordered_map<int, Item>& GetItemsForEdit() { return items_; }
	int& GetUsedID() { return usedID_; }

	std::unordered_map<std::string, float>& GetBaseParamsForEdit() { return baseParam_; }
	const std::unordered_map<std::string, float>& GetBaseParamsForEdit() const { return baseParam_; }

#endif

private:


	void SaveModel();
	void LoadModel();

	void SaveItem();
	void LoadItem();

	void SaveBaseParam();
	void LoadBaseParam();

	void ResolveAllModelIDs();

	//パラメータ一覧と基礎値
	std::unordered_map<std::string, float> baseParam_;
	std::unordered_map<int, Item> items_;

	int usedID_ = 0;

	// モデルIDとパスのマップ
	std::map<int, std::string> modelIDtoName_;

	SHEngine::ModelManager* modelManager_ = nullptr;
	BinaryManager binaryManager_;
	JsonManager jsonManager_;

	static inline const std::string modelFile_ = "ModelIDMap.bin";
	static inline const std::string itemFile_ = "ItemData.bin";
	static inline const std::string baseParamFile_ = "BaseParamData.bin";
};
