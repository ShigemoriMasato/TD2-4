#include"UnitEffectManager.h"

void UnitEffectManager::Initialize(DrawData oreItemData, int oreItemTexture) {

	// メモリを確保
	oreItemObjects_.resize(100);

	for (int32_t i = 0; i < 100; ++i) {
		oreItemObjects_[i] = std::make_unique<DefaultObject>();
		oreItemObjects_[i]->Initialize(oreItemData, oreItemTexture);
		oreItemObjects_[i]->transform_.scale = { 1.0f,1.0f,1.0f };
		oreItemObjects_[i]->material_.isActive = true;
	}
}

void UnitEffectManager::Update() {

	// 更新処理
	for (int32_t i = 0; i < index_+1; ++i) {
		oreItemObjects_[i]->Update();
	}
}

void UnitEffectManager::AddOreItem(const Vector3& pos) {
	index_++;
	oreItemObjects_[index_]->transform_.position = pos;
}

void UnitEffectManager::Draw(Window* window, const Matrix4x4& vpMatrix) {

	// 描画
	for (int32_t i = 0; i < index_+1; ++i) {
		oreItemObjects_[i]->Draw(window,vpMatrix);
	}
}