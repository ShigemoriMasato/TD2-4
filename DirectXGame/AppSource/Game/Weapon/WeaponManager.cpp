#include "WeaponManager.h"

void WeaponManager::InitializeData() {
	database_[0] = {
	    0,             // ID
	    "Short_Sword", // 名前
	    10.0f,         // 基本ダメージ
	    1.0f,          // 攻撃速度
	    30.0f,         // 値段
	    4,             // サイズ
	    1              // レア度
	};
}

WeaponData* WeaponManager::GetWeapon(int id) {
	if (database_.find(id) != database_.end()) {
		return &database_[id];
	}
	return nullptr;
}

//void WeaponManager::SaveModel() {
//	int size = static_cast<int>(modelIDtoName_.size());
//	binaryManager_.RegisterOutput(&size);
//	for (const auto& [id, name] : modelIDtoName_) {
//		binaryManager_.RegisterOutput(&id);
//		binaryManager_.RegisterOutput(&name);
//	}
//	binaryManager_.Write(modelFile_);
//}
//
//void WeaponManager::LoadModel() {
//	auto data = binaryManager_.Read(modelFile_);
//	if (data.empty()) {
//		return;
//	}
//	int size = binaryManager_.Reverse<int>(data);
//	for (int i = 0; i < size; ++i) {
//		int id = binaryManager_.Reverse<int>(data);
//		std::string name = binaryManager_.Reverse<std::string>(data);
//		modelIDtoName_[id] = name;
//	}
//}
//
//void WeaponManager::SaveItem() {
//	int size = static_cast<int>(items_.size());
//	binaryManager_.RegisterOutput(&size);
//	for (auto& item : items_) {
//
//		std::string tmp = ConvertString(item.name);
//		binaryManager_.RegisterOutput(&tmp);
//
//		int category = static_cast<int>(item.category);
//		binaryManager_.RegisterOutput(&category);
//
//		binaryManager_.RegisterOutput(&item.effect);
//
//		int mapDataSize = static_cast<int>(item.mapData.size());
//		binaryManager_.RegisterOutput(&mapDataSize);
//
//		for (auto& [x, y] : item.mapData) {
//			binaryManager_.RegisterOutput(&x);
//			binaryManager_.RegisterOutput(&y);
//		}
//
//		int buffsSize = static_cast<int>(item.params.size());
//		binaryManager_.RegisterOutput(&buffsSize);
//
//		for (auto& buff : item.params) {
//			std::string tmp = buff.first;
//			binaryManager_.RegisterOutput(&tmp);
//			binaryManager_.RegisterOutput(&buff.second);
//		}
//
//		binaryManager_.RegisterOutput(&item.weaponID);
//	}
//	binaryManager_.Write(itemFile_);
//}
//
//void WeaponManager::LoadItem() {
//	auto data = binaryManager_.Read(itemFile_);
//	if (data.empty()) {
//		return;
//	}
//
//	int size = binaryManager_.Reverse<int>(data);
//	for (int i = 0; i < size; ++i) {
//		Item item;
//		item.name = ConvertString(binaryManager_.Reverse<std::string>(data));
//
//		int category = binaryManager_.Reverse<int>(data);
//		item.category = static_cast<Category>(category);
//
//		item.effect = binaryManager_.Reverse<uint32_t>(data);
//
//		int mapDataSize = binaryManager_.Reverse<int>(data);
//
//		for (int j = 0; j < mapDataSize; ++j) {
//			int x = binaryManager_.Reverse<int>(data);
//			int y = binaryManager_.Reverse<int>(data);
//			item.mapData.emplace_back(x, y);
//		}
//
//		int buffsSize = binaryManager_.Reverse<int>(data);
//		item.params = baseParam_; // 基礎値をコピー
//
//		for (int j = 0; j < buffsSize; ++j) {
//			std::string name = binaryManager_.Reverse<std::string>(data);
//			float value = binaryManager_.Reverse<float>(data);
//			item.params[name] = value;
//		}
//
//		item.weaponID = binaryManager_.Reverse<int>(data);
//		items_.push_back(item);
//	}
//}
//
//void WeaponManager::SaveBaseParam() {
//	int size = static_cast<int>(baseParam_.size());
//	binaryManager_.RegisterOutput(&size);
//	for (const auto& param : baseParam_) {
//		binaryManager_.RegisterOutput(&param.first);
//		binaryManager_.RegisterOutput(&param.second);
//	}
//	binaryManager_.Write(baseParamFile_);
//}
//
//void WeaponManager::LoadBaseParam() {
//	auto data = binaryManager_.Read(baseParamFile_);
//	if (data.empty()) {
//		return;
//	}
//	int size = binaryManager_.Reverse<int>(data);
//	for (int i = 0; i < size; ++i) {
//		std::string name = binaryManager_.Reverse<std::string>(data);
//		float value = binaryManager_.Reverse<float>(data);
//		baseParam_[name] = value;
//	}
//}
