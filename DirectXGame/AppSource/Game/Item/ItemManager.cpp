#include "ItemManager.h"
#include "Item.h"
#include <Utility/ConvertString.h>
#include <imgui/imgui.h>
#include <algorithm>
#include <unordered_set>

#ifdef USE_IMGUI
#include "ItemEditor.h"
#endif

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

	if (items_.empty())
	{
		Item defaultItem{};
		defaultItem.name = L"Default Item";
		defaultItem.category = Category::Item;
		for (int r = 0; r < 4; ++r)
		{
			defaultItem.ranks[r].price = 0;
			defaultItem.ranks[r].effect = 0u;
			defaultItem.ranks[r].params = baseParam_;
		}
		defaultItem.modelPath = "Assets/EngineResource/Model/Cube";
		items_.push_back(std::move(defaultItem));
	}

	ResolveAllModelIDs();
}

const Item& ItemManager::GetItem(std::wstring itemName) const
{
	for (const auto& item : items_)
	{
		if (item.name == itemName)
		{
			return item;
		}
	}
	throw std::runtime_error("Item not found: " + ConvertString(itemName));
}

const Item& ItemManager::GetItem(int index) const
{
	int targetIndex = index % static_cast<int>(items_.size());
	return items_[targetIndex];
}

int ItemManager::ResolveModelID(const Item& item)
{
	// modelPathが空ならエラー
	if (item.modelPath.empty()) return -1;

	return modelManager_->LoadModel(item.modelPath);
}

void ItemManager::ResolveAllModelIDs()
{
	for (auto& item : items_)
	{
		item.modelID = ResolveModelID(item);
	}
}


void ItemManager::DrawImGui()
{
#ifdef USE_IMGUI
	static ItemEditor editor;
	editor.Draw(*this);
#endif
}

//=================================================================
// セーブとロード
//=================================================================

void ItemManager::SaveModel()
{
	int size = static_cast<int>(modelIDtoName_.size());
	binaryManager_.RegisterOutput(&size);
	for (const auto& [id, name] : modelIDtoName_)
	{
		binaryManager_.RegisterOutput(&id);
		binaryManager_.RegisterOutput(&name);
	}
	binaryManager_.Write(modelFile_);
}

void ItemManager::LoadModel()
{
	auto data = binaryManager_.Read(modelFile_);
	if (data.empty())
	{
		return;
	}
	int size = binaryManager_.Reverse<int>(data);
	for (int i = 0; i < size; ++i)
	{
		int id = binaryManager_.Reverse<int>(data);
		std::string name = binaryManager_.Reverse<std::string>(data);
		modelIDtoName_[id] = name;
	}
}

void ItemManager::SaveItem()
{
	int size = static_cast<int>(items_.size());
	binaryManager_.RegisterOutput(&size);
	for (auto& item : items_)
	{
		// name
		std::string tmpName = ConvertString(item.name);
		binaryManager_.RegisterOutput(&tmpName);

		// category
		int category = static_cast<int>(item.category);
		binaryManager_.RegisterOutput(&category);

		// mapData
		int mapDataSize = static_cast<int>(item.mapData.size());
		binaryManager_.RegisterOutput(&mapDataSize);
		for (auto& [x, y] : item.mapData)
		{
			binaryManager_.RegisterOutput(&x);
			binaryManager_.RegisterOutput(&y);
		}

		// 見た目
		binaryManager_.RegisterOutput(&item.modelPath);
		binaryManager_.RegisterOutput(&item.weaponID);

		// ranks（可変）
		for (int r = 0; r < 4; ++r)
		{
			// price
			binaryManager_.RegisterOutput(&item.ranks[r].price);
			// effect
			binaryManager_.RegisterOutput(&item.ranks[r].effect);

			int buffsSize = static_cast<int>(item.ranks[r].params.size());
			binaryManager_.RegisterOutput(&buffsSize);

			// params
			for (auto& buff : item.ranks[r].params)
			{
				std::string key = buff.first;
				binaryManager_.RegisterOutput(&key);
				binaryManager_.RegisterOutput(&buff.second);
			}
		}
	}
	binaryManager_.Write(itemFile_);
}

void ItemManager::LoadItem()
{
	auto data = binaryManager_.Read(itemFile_);
	if (data.empty())
	{
		return;
	}

	int size = binaryManager_.Reverse<int>(data);
	for (int i = 0; i < size; ++i)
	{
		Item item{};

		// name
		item.name = ConvertString(binaryManager_.Reverse<std::string>(data));

		// category
		int category = binaryManager_.Reverse<int>(data);
		item.category = static_cast<Category>(category);

		// mapData
		int mapDataSize = binaryManager_.Reverse<int>(data);
		for (int j = 0; j < mapDataSize; ++j)
		{
			int x = binaryManager_.Reverse<int>(data);
			int y = binaryManager_.Reverse<int>(data);
			item.mapData.emplace_back(x, y);
		}

		// 見た目
		item.modelPath = binaryManager_.Reverse<std::string>(data);
		item.weaponID = binaryManager_.Reverse<int>(data);

		// ranks
		for (int r = 0; r < 4; ++r)
		{
			auto& rd = item.ranks[r];

			rd.price = binaryManager_.Reverse<int>(data);
			rd.effect = binaryManager_.Reverse<uint32_t>(data);

			int buffsSize = binaryManager_.Reverse<int>(data);
			rd.params = baseParam_;
			for (int j = 0; j < buffsSize; ++j)
			{
				std::string name = binaryManager_.Reverse<std::string>(data);
				float value = binaryManager_.Reverse<float>(data);
				rd.params[name] = value;
			}
		}

		items_.push_back(item);
	}
}

void ItemManager::SaveBaseParam()
{
	int size = static_cast<int>(baseParam_.size());
	binaryManager_.RegisterOutput(&size);
	for (const auto& param : baseParam_)
	{
		binaryManager_.RegisterOutput(&param.first);
		binaryManager_.RegisterOutput(&param.second);
	}
	binaryManager_.Write(baseParamFile_);
}

void ItemManager::LoadBaseParam()
{
	auto data = binaryManager_.Read(baseParamFile_);
	if (data.empty())
	{
		return;
	}
	int size = binaryManager_.Reverse<int>(data);
	for (int i = 0; i < size; ++i)
	{
		std::string name = binaryManager_.Reverse<std::string>(data);
		float value = binaryManager_.Reverse<float>(data);
		baseParam_[name] = value;
	}
}

