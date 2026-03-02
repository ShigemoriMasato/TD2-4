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

class ItemManager {
public:

	~ItemManager();
	void Initialize(SHEngine::ModelManager* modelManager);
	void DrawImGui();

	// アイテムの名前からアイテムデータを取得(名前リストがどこかにある想定)
	const Item& GetItem(std::wstring itemName) const;
	const Item& GetItem(int index) const;

	// 描画等で必要になったときに ID を解決する
	int ResolveModelID(Item& item);

#ifdef USE_IMGUI 

	/// Editer用関数
	std::vector<Item>& GetItemsForEdit() { return items_; }
	const std::vector<Item>& GetItemsForEdit() const { return items_; }

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
	std::vector<Item> items_;

	// モデルIDとパスのマップ
	std::map<int, std::string> modelIDtoName_;

	SHEngine::ModelManager* modelManager_ = nullptr;
	BinaryManager binaryManager_;

	static inline const std::string modelFile_ = "ModelIDMap.bin";
	static inline const std::string itemFile_ = "ItemData.bin";
	static inline const std::string baseParamFile_ = "BaseParamData.bin";
};
