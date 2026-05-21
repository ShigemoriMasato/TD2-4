#include "Weapon.h"

Weapon::Weapon(std::string dataFilePath) {
	if (binManager_.Boot(dataFilePath)) {
		return;
	}

	data_.modelFilePath = binManager_.Reverse<std::string>();
	data_.regularMatrix = binManager_.Reverse<Matrix4x4>();
	int keyCount = binManager_.Reverse<int>();
	for (int i = 0; i < keyCount; ++i) {
		Key key;
		key.time = binManager_.Reverse<float>();
		int matrixCount = binManager_.Reverse<int>();
		for (int j = 0; j < matrixCount; ++j) {
			key.matrices.push_back(binManager_.Reverse<Matrix4x4>());
		}
		data_.attackAnimation.push_back(key);
	}
}

void Weapon::Update(float deltaTime) {
	if (timer_ > animationTime_) {
		floatTimer_ += deltaTime;
		renderData_.world = data_.regularMatrix;
		float floatingY = std::sin(floatTimer_ * 2.0f) * 0.1f;
		renderData_.world.m[3][1] += floatingY;
		return;
	}

	for (size_t i = 0; i < data_.attackAnimation.size(); ++i) {
		if (timer_ < data_.attackAnimation[i].time) {
			break;
		}
	}
}

void Weapon::DataSetting() {
	if (data_.attackAnimation.empty()) {
		return;
	}

	//攻撃アニメーションを再生するのにかかる時間を取得
	animationTime_ = data_.attackAnimation.back().time;

	int handle = modelManager_->LoadModel(data_.modelFilePath);
	auto& data = modelManager_->GetNodeModelData(handle);
	renderData_.modelData = data;
	renderData_.drawData = drawDataManager_->GetDrawData(data.drawDataIndex);
}
