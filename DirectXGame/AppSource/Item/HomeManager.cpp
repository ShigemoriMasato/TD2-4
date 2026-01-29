#include"HomeManager.h"

void HomeManager::Initialize(const DrawData& goldOreDrawData, int texture, std::vector<Vector3> list) {

	drawData_ = goldOreDrawData;
	texture_ = texture;

	// メモリを確保
	homeList_.reserve(5);

	// 拠点を配置する
	for (size_t i = 0; i < list.size(); ++i) {
		AddHome(list[i]);
	}

	// 更新処理
	Update();
}

void HomeManager::Update() {

	// 更新処理
	for (auto& home : homeList_) {
		home->Update();
	}
}

void HomeManager::Draw(Window* window, const Matrix4x4& vpMatrix) {

	// 拠点を描画する
	for (auto& home : homeList_) {
		home->Draw(window,vpMatrix);
	}
}

void HomeManager::AddHome(const Vector3& pos, const float rotY) {

	std::unique_ptr<OreHome> home = std::make_unique<OreHome>();
	home->Initialize(drawData_, texture_,pos,rotY);
	homeList_.push_back(std::move(home));
}