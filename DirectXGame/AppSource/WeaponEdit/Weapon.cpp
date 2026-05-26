#include "Weapon.h"
#include <Utility/MatrixFactory.h>

Weapon::Weapon() {
}

void Weapon::Update(float deltaTime) {
	if (timer_ > animationTime_) {
		floatTimer_ += deltaTime;
		for (const auto& regularKey : data_.regularKey) {
			renderData_.world *= Matrix::MakeAffineMatrix(regularKey.scale, regularKey.rotate, regularKey.position);
		}
		float floatingY = std::sin(floatTimer_ * 2.0f) * 0.1f;
		renderData_.world.m[3][1] += floatingY;
		return;
	}

	for (size_t i = 0; i < data_.attackAnimation.size(); ++i) {
		if (timer_ < data_.attackAnimation[i].time) {

			//アニメーションするコード

			break;
		}
	}
}

void Weapon::SetWeaponData(const std::string& dataFilePath) {
}

void Weapon::DataSetting() {
	if (data_.attackAnimation.empty()) {
		return;
	}

	//攻撃アニメーションを再生するのにかかる時間を取得
	animationTime_ = data_.attackAnimation.back().time;

	attackAnimation_.clear();
	attackAnimation_.resize(data_.attackAnimation.size() + 1);
	

	int handle = modelManager_->LoadModel(data_.modelFilePath);
	auto& data = modelManager_->GetNodeModelData(handle);
	renderData_.modelData = data;
	renderData_.drawData = drawDataManager_->GetDrawData(data.drawDataIndex);
}

void Weapon::Data::Load(const std::string& filePath) {
	BinaryManager itemBin;
	if (!itemBin.Boot("Weapons/" + filePath + ".bin")) {
		assert(false && "Weapon::Load: Failed to load weapon data");
		return;
	}
	modelFilePath = itemBin.Reverse<std::string>();
	int regularKeyCount = itemBin.Reverse<int>();
	regularKey.resize(regularKeyCount);
	for (int i = 0; i < regularKeyCount; ++i) {
		regularKey[i] = itemBin.Reverse<Transform>();
	}
	int keyCount = itemBin.Reverse<int>();
	for (int j = 0; j < keyCount; ++j) {
		Weapon::Key key;
		key.time = itemBin.Reverse<float>();
		int dataCount = itemBin.Reverse<int>();
		key.data.resize(dataCount);
		for (int k = 0; k < dataCount; ++k) {
			key.data[k] = itemBin.Reverse<Transform>();
		}
		attackAnimation.push_back(key);
	}
}

void Weapon::Data::Save(const std::string& path) const {
	BinaryManager binManager;

	binManager.Register(&modelFilePath);

	int regularKeyCount = int(regularKey.size());
	binManager.Register(&regularKeyCount);
	for (const auto& regularKey : regularKey) {
		binManager.Register(&regularKey);
	}

	int keyCount = int(attackAnimation.size());
	binManager.Register(&keyCount);
	for (const auto& key : attackAnimation) {
		binManager.Register(&key.time);
		int dataCount = int(key.data.size());
		binManager.Register(&dataCount);
		for (const auto& transform : key.data) {
			binManager.Register(&transform);
		}
	}

	binManager.Write("Weapons/" + path + ".bin");
}
