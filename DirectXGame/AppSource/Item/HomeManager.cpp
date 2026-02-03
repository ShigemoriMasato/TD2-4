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
	for (auto& home : homeList_) {
		home->Update();
	}
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

void HomeManager::SetAnimation(const Vector3& pos) {

	// 指定された拠点をアニメーションさせる
	for (auto& home : homeList_) {

		if (pos.x == home->homeObject_->transform_.position.x &&
			pos.z == home->homeObject_->transform_.position.z) {
			home->StartAnimation();
		}
	}
}

void HomeManager::SetMinMapPos(MiniMap* miniMap, const DrawData& spreteDrawData, int texture) {

	iconObjects_.resize(homeList_.size());

	for (size_t i = 0; i < homeList_.size(); ++i) {
		iconObjects_[i] = std::make_unique<SpriteObject>();
		iconObjects_[i]->Initialize(spreteDrawData, { 128.0f,128.0f });
		MarkerResult marker = miniMap->GetMarkerInfo(homeList_[i]->homeObject_->transform_.position, 36.0f);
		iconObjects_[i]->transform_.position = { marker.position.x,marker.position.y,0.0f };
		iconObjects_[i]->color_ = { 1.0f,1.0f,1.0f,1.0f };
		iconObjects_[i]->SetTexture(texture);
		iconObjects_[i]->Update();
	}
}

void HomeManager::DrawIcon(Window* window, const Matrix4x4& vpMatrix) {

	// アイコンを描画
	for (auto& icon : iconObjects_) {
		icon->Draw(window, vpMatrix);
	}
}