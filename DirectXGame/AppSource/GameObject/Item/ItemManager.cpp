#include "ItemManager.h"
#include "Item.h"
#include <Utility/ConvertString.h>
#include <imgui/imgui.h>
#include <algorithm>
#include <unordered_set>

ItemManager::ItemManager() {}

ItemManager::~ItemManager()
{
	SaveBaseParam();
	SaveItem();
	SaveModel();
}

void ItemManager::Initialize(SHEngine::ModelManager* modelManager)
{
	modelManager_ = modelManager;

	LoadBaseParam();
	LoadItem();
	LoadModel();

	ResolveAllModelIDs();

	editor.CreateItemFromModel(*this);
}

const Item& ItemManager::GetItem(std::wstring itemName) const
{
	for (const auto& [id, item] : items_)
	{
		if (item.name == itemName)
		{
			return item;
		}
	}
	throw std::runtime_error("Item not found: " + ConvertString(itemName));
}

const Item& ItemManager::GetItem(int index)
{
	auto it = items_.find(index);
	if(it == items_.end())
	{
		throw std::runtime_error("Item not found with ID: " + std::to_string(index));
	}
	return items_[index];
}

int ItemManager::ResolveModelID(Item& item)
{
	// modelPathが空ならエラー
	if (item.modelPath.empty()) return -1;

	int tempID = modelManager_->LoadModel(item.modelPath);

	return tempID;
}

void ItemManager::ResolveAllModelIDs()
{
	for (auto& [id, item] : items_)
	{
		item.modelID = ResolveModelID(item);
	}
}


void ItemManager::DrawImGui()
{
#ifdef USE_IMGUI
	editor.Draw(*this);
#endif
}

//=================================================================
// セーブとロード
//=================================================================

void ItemManager::SaveModel()
{
	int size = static_cast<int>(modelIDtoName_.size());
	binaryManager_.Register(&size);
	for (const auto& [id, name] : modelIDtoName_)
	{
		binaryManager_.Register(&id);
		binaryManager_.Register(&name);
	}
	binaryManager_.Write(modelFile_);
}

void ItemManager::LoadModel()
{
	if (!binaryManager_.Boot(modelFile_))
	{
		return;
	}
	int size = binaryManager_.Reverse<int>();
	for (int i = 0; i < size; ++i)
	{
		int id = binaryManager_.Reverse<int>();
		std::string name = binaryManager_.Reverse<std::string>();
		modelIDtoName_[id] = name;
	}
}

void ItemManager::SaveItem()
{
	int size = static_cast<int>(items_.size());
	binaryManager_.Register(&size);
	for (auto& [id, item] : items_)
	{
		// name
		std::string tmpName = ConvertString(item.name);
		binaryManager_.Register(&tmpName);

		// ID
		binaryManager_.Register(&item.id);

		// category
		int category = static_cast<int>(item.category);
		binaryManager_.Register(&category);

		// mapData
		int mapDataSize = static_cast<int>(item.mapData.size());
		binaryManager_.Register(&mapDataSize);
		for (auto& [x, y] : item.mapData)
		{
			binaryManager_.Register(&x);
			binaryManager_.Register(&y);
		}

		// 見た目
		binaryManager_.Register(&item.modelPath);
		binaryManager_.Register(&item.weaponID);
		binaryManager_.Register(&item.visualOffsetCells);

		//Active
		binaryManager_.Register(&item.isActive);

		// ranks（可変）
		for (int r = 0; r < 4; ++r)
		{
			// price
			binaryManager_.Register(&item.ranks[r].price);
			// effect
			binaryManager_.Register(&item.ranks[r].effect);

			int buffsSize = static_cast<int>(item.ranks[r].params.size());
			binaryManager_.Register(&buffsSize);

			// params
			for (auto& buff : item.ranks[r].params)
			{
				std::string key = buff.first;
				binaryManager_.Register(&key);
				binaryManager_.Register(&buff.second);
			}
		}
	}
	binaryManager_.Write(itemFile_);
}

void ItemManager::LoadItem()
{
	// 指定したファイルを読み込み
	jsonManager_.Boot(itemFile_);

	if (!binaryManager_.Boot(itemFile_))
	{
		return;
	}

	int size = binaryManager_.Reverse<int>();
	for (int i = 0; i < size; ++i)
	{
		Item item{};

		// name
		item.name = ConvertString(binaryManager_.Reverse<std::string>());

		// ID
		item.id = binaryManager_.Reverse<int>();
		usedID_ = std::max(usedID_, item.id); // 読み込んだIDをもとにusedID_を更新
		if (item.id == -1) {
			item.id = usedID_++;
		}

		// category
		int category = binaryManager_.Reverse<int>();
		item.category = static_cast<Category>(category);

		// mapData
		int mapDataSize = binaryManager_.Reverse<int>();
		for (int j = 0; j < mapDataSize; ++j)
		{
			int x = binaryManager_.Reverse<int>();
			int y = binaryManager_.Reverse<int>();
			item.mapData.push_back({ x, y });
		}

		// 見た目
		item.modelPath = binaryManager_.Reverse<std::string>();
		item.weaponID = binaryManager_.Reverse<int>();
		item.visualOffsetCells = binaryManager_.Reverse<Vector2>();

		//Active
		item.isActive = binaryManager_.Reverse<bool>();

		// ranks
		for (int r = 0; r < 4; ++r)
		{
			auto& rd = item.ranks[r];

			rd.price = binaryManager_.Reverse<int>();
			rd.effect = binaryManager_.Reverse<uint32_t>();

			int buffsSize = binaryManager_.Reverse<int>();
			for (int j = 0; j < buffsSize; ++j)
			{
				std::string name = binaryManager_.Reverse<std::string>();
				float value = binaryManager_.Reverse<float>();
			}
		}

		items_[item.id] = item;
	}
}

void ItemManager::SaveBaseParam()
{
	int size = static_cast<int>(baseParam_.size());
	binaryManager_.Register(&size);
	for (const auto& param : baseParam_)
	{
		std::string name = param.first;
		float value = param.second;
		binaryManager_.Register(&name);
		binaryManager_.Register(&value);
	}
	binaryManager_.Write(baseParamFile_);
}

void ItemManager::LoadBaseParam()
{
	if (!binaryManager_.Boot(baseParamFile_))
	{
		return;
	}
	int size = binaryManager_.Reverse<int>();
	for (int i = 0; i < size; ++i)
	{
		std::string name = binaryManager_.Reverse<std::string>();
		float value = binaryManager_.Reverse<float>();
		baseParam_[name] = value;
	}
}

