#include "ItemManager.h"
#include <Utility/ConvertString.h>

ItemManager::~ItemManager() {
	SaveBaseParam();
	SaveItem();
	SaveModel();
}

void ItemManager::Initialize(SHEngine::ModelManager* modelManager) {
	modelManager_ = modelManager;

	LoadBaseParam();
	LoadItem();
	LoadModel();
}

const Item& ItemManager::GetItem(std::wstring itemName) const {
	for(const auto& item : items_) {
		if (item.name == itemName) {
			return item;
		}
	}
}

void ItemManager::DrawImGui() {
}

//=================================================================
// セーブとロード
//=================================================================

void ItemManager::SaveModel() {
	int size = static_cast<int>(modelIDtoName_.size());
	binaryManager_.RegisterOutput(&size);
	for (const auto& [id, name] : modelIDtoName_) {
		binaryManager_.RegisterOutput(&id);
		binaryManager_.RegisterOutput(&name);
	}
	binaryManager_.Write(modelFile_);
}

void ItemManager::LoadModel() {
	auto data = binaryManager_.Read(modelFile_);
	if (data.empty()) {
		return;
	}
	int size = binaryManager_.Reverse<int>(data);
	for (int i = 0; i < size; ++i) {
		int id = binaryManager_.Reverse<int>(data);
		std::string name = binaryManager_.Reverse<std::string>(data);
		modelIDtoName_[id] = name;
	}
}

void ItemManager::SaveItem() {
	int size = static_cast<int>(items_.size());
	binaryManager_.RegisterOutput(&size);
	for (const auto& item : items_) {
		binaryManager_.RegisterOutput(&ConvertString(item.name));
		int category = static_cast<int>(item.category);
		binaryManager_.RegisterOutput(&category);
		binaryManager_.RegisterOutput(&item.effect);
		int mapDataSize = static_cast<int>(item.mapData.size());
		binaryManager_.RegisterOutput(&mapDataSize);
		for (const auto& [x, y] : item.mapData) {
			binaryManager_.RegisterOutput(&x);
			binaryManager_.RegisterOutput(&y);
		}
		int buffsSize = static_cast<int>(item.buffs.size());
		binaryManager_.RegisterOutput(&buffsSize);
		for (const auto& buff : item.buffs) {
			binaryManager_.RegisterOutput(&buff.name);
			binaryManager_.RegisterOutput(&buff.value);
		}
		binaryManager_.RegisterOutput(&item.weaponID);
	}
	binaryManager_.Write(itemFile_);
}

void ItemManager::LoadItem() {
	auto data = binaryManager_.Read(itemFile_);
	if (data.empty()) {
		return;
	}

	int size = binaryManager_.Reverse<int>(data);
	for (int i = 0; i < size; ++i) {
		Item item;
		item.name = ConvertString(binaryManager_.Reverse<std::string>(data));
		int category = binaryManager_.Reverse<int>(data);
		item.category = static_cast<Category>(category);
		item.effect = binaryManager_.Reverse<uint32_t>(data);
		int mapDataSize = binaryManager_.Reverse<int>(data);
		for (int j = 0; j < mapDataSize; ++j) {
			int x = binaryManager_.Reverse<int>(data);
			int y = binaryManager_.Reverse<int>(data);
			item.mapData.emplace_back(x, y);
		}
		int buffsSize = binaryManager_.Reverse<int>(data);
		for (int j = 0; j < buffsSize; ++j) {
			ItemParam buff;
			buff.name = binaryManager_.Reverse<std::string>(data);
			buff.value = binaryManager_.Reverse<float>(data);
			item.buffs.push_back(buff);
		}
		item.weaponID = binaryManager_.Reverse<int>(data);
		items_.push_back(item);
	}
}

void ItemManager::SaveBaseParam() {
	int size = static_cast<int>(baseParam_.size());
	binaryManager_.RegisterOutput(&size);
	for (const auto& param : baseParam_) {
		binaryManager_.RegisterOutput(&param.name);
		binaryManager_.RegisterOutput(&param.value);
	}
}

void ItemManager::LoadBaseParam() {
	auto data = binaryManager_.Read(baseParamFile_);
	if (data.empty()) {
		return;
	}
	int size = binaryManager_.Reverse<int>(data);
	for (int i = 0; i < size; ++i) {
		ItemParam param;
		param.name = binaryManager_.Reverse<std::string>(data);
		param.value = binaryManager_.Reverse<float>(data);
		baseParam_.push_back(param);
	}
}
