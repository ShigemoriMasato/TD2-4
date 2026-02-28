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
		defaultItem.effect = 0u;
		defaultItem.params = baseParam_;
		items_.push_back(std::move(defaultItem));
	}
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

		std::string tmp = ConvertString(item.name);
		binaryManager_.RegisterOutput(&tmp);

		int category = static_cast<int>(item.category);
		binaryManager_.RegisterOutput(&category);

		binaryManager_.RegisterOutput(&item.effect);

		int mapDataSize = static_cast<int>(item.mapData.size());
		binaryManager_.RegisterOutput(&mapDataSize);

		for (auto& [x, y] : item.mapData)
		{
			binaryManager_.RegisterOutput(&x);
			binaryManager_.RegisterOutput(&y);
		}

		int buffsSize = static_cast<int>(item.params.size());
		binaryManager_.RegisterOutput(&buffsSize);

		for (auto& buff : item.params)
		{
			std::string tmp = buff.first;
			binaryManager_.RegisterOutput(&tmp);
			binaryManager_.RegisterOutput(&buff.second);
		}

		binaryManager_.RegisterOutput(&item.weaponID);
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
		Item item;
		item.name = ConvertString(binaryManager_.Reverse<std::string>(data));

		int category = binaryManager_.Reverse<int>(data);
		item.category = static_cast<Category>(category);

		item.effect = binaryManager_.Reverse<uint32_t>(data);

		int mapDataSize = binaryManager_.Reverse<int>(data);

		for (int j = 0; j < mapDataSize; ++j)
		{
			int x = binaryManager_.Reverse<int>(data);
			int y = binaryManager_.Reverse<int>(data);
			item.mapData.emplace_back(x, y);
		}

		int buffsSize = binaryManager_.Reverse<int>(data);
		item.params = baseParam_; // 基礎値をコピー

		for (int j = 0; j < buffsSize; ++j)
		{
			std::string name = binaryManager_.Reverse<std::string>(data);
			float value = binaryManager_.Reverse<float>(data);
			item.params[name] = value;
		}

		item.weaponID = binaryManager_.Reverse<int>(data);
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

