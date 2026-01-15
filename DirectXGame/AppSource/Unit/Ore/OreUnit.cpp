#include"OreUnit.h"

void OreUnit::Initialize(MapChipField* mapChipField, DrawData drawData, const Vector3& pos) {
	// マップデータ
	mapChipField_ = mapChipField;

	// オブジェクトを初期化
	object_ = std::make_unique<OreUnitObject>();
	object_->Initialize(drawData);

	// 初期位置を設定
	object_->transform_.position = pos;
}

void OreUnit::Update() {



	// 更新処理
	object_->Update();
}

void OreUnit::Draw(Window* window, const Matrix4x4& vpMatrix) {
	// 描画
	object_->Draw(window, vpMatrix);
}