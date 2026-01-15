#include"PlayerUnit.h"

void PlayerUnit::Initialize(MapChipField* mapChipField, DrawData drawData, const Vector3& pos, KeyManager* keyManager) {
	// マップデータ
	mapChipField_ = mapChipField;

	// 入力を取得
	keyManager_ = keyManager;

	// オブジェクトを初期化
	object_ = std::make_unique<PlayerUnitObject>();
	object_->Initialize(drawData);

	// 初期位置を設定
	object_->transform_.position = pos;
}

void PlayerUnit::Update() {



	// 更新処理
	object_->Update();
}

void PlayerUnit::Draw(Window* window, const Matrix4x4& vpMatrix) {
	// 描画
	object_->Draw(window, vpMatrix);
}

void PlayerUnit::ProcessMoveInput() {
	
	auto key = keyManager_->GetKeyStates();

	if (key[Key::Up]) {

	}

	if (key[Key::Down]) {

	}

	if (key[Key::Left]) {

	}

	if (key[Key::Right]) {

	}
}