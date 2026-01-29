#include"OreHome.h"

void OreHome::Initialize(const DrawData& homeDrawData, int texture, const Vector3& pos, const float& rotY) {
	// オブジェクトの初期化
	homeObject_ = std::make_unique<DefaultObject>();
	homeObject_->Initialize(homeDrawData, texture);
	homeObject_->transform_.position = pos;
	homeObject_->transform_.rotate.y = rotY;
}

void OreHome::Update() {

	// 更新処理
	homeObject_->Update();
}

void OreHome::Draw(Window* window, const Matrix4x4& vpMatrix) {

	// 描画
	homeObject_->Draw(window, vpMatrix);
}

void OreHome::Animation() {

}