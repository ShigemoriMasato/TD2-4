#include"HomeManager.h"

void HomeManager::Initialize(const DrawData& goldOreDrawData, int texture, std::vector<Vector3> list, DrawData spriteDrawData) {

	drawData_ = goldOreDrawData;
	texture_ = texture;

	// メモリを確保
	homeList_.reserve(5);

	// 拠点を配置する
	for (size_t i = 0; i < list.size(); ++i) {
		AddHome(list[i]);
	}

	homeMarkList_.resize(5);
	for (size_t i = 0; i < homeMarkList_.size(); ++i) {
		homeMarkList_[i].arrowObject_ = std::make_unique<SpriteObject>();
		homeMarkList_[i].arrowObject_->Initialize(spriteDrawData, {64.0f,64.0f});
		homeMarkList_[i].arrowObject_->transform_.position = {-128.0f,0.0f,0.0f};
		homeMarkList_[i].arrowObject_->color_ = { 1.0f,0.0f,0.0f,1.0f };
	}

	// 更新処理
	for (auto& home : homeList_) {
		home->Update();
	}
}

void HomeManager::Update() {

	activeIndex_ = -1;

	// 更新処理
	for (auto& home : homeList_) {
		// マークを追加する
		AddMark(home->homeObject_->transform_.position);

		home->Update();
	}

	// マークを更新
	if (activeIndex_ != -1) {
		for (size_t i = 0; i <= activeIndex_; ++i) {
			homeMarkList_[i].arrowObject_->Update();
		}
	}
}

void HomeManager::Draw(Window* window, const Matrix4x4& vpMatrix) {

	// 拠点を描画する
	for (auto& home : homeList_) {
		home->Draw(window,vpMatrix);
	}
}

void HomeManager::DrawUI(Window* window, const Matrix4x4& vpMatrix) {

	// 家マークUI
	if (activeIndex_ != -1) {
		for (size_t i = 0; i <= activeIndex_; ++i) {
			homeMarkList_[i].arrowObject_->Draw(window, vpMatrix);
		}
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

void HomeManager::AddMark(const Vector3& pos) {

	MarkerResult marker = cameraController_->GetMarkerInfo(pos, 48.0f);

	if (!marker.isVisible) {

		activeIndex_++;

		homeMarkList_[activeIndex_].arrowObject_->transform_.position.x = marker.position.x;
		homeMarkList_[activeIndex_].arrowObject_->transform_.position.y = marker.position.y;

		homeMarkList_[activeIndex_].arrowObject_->transform_.rotate.z = marker.rotation;
	}
}