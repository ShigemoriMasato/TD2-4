#include"GoldOre.h"
#include <Common/DebugParam/GameParamEditor.h>
void GoldOre::Init(DrawData drawData, const Vector3& pos) {
	
	// オブジェクトを初期化
	object_ = std::make_unique<OreObject>();
	object_->Initialize(drawData);

	// 初期位置を設定
	object_->transform_.position = pos;

	// 設定
	quadCollider_.topLeft = { -size_.x * 0.5f + object_->transform_.position.x, -size_.z * 0.5f + object_->transform_.position.z};
	quadCollider_.bottomRight = { size_.x * 0.5f + object_->transform_.position.x, size_.z * 0.5f + object_->transform_.position.z };

	// 当たり判定の要素を設定
	CollConfig config;
	config.colliderInfo = &quadCollider_;
	config.isActive = true;
	config.ownTag = CollTag::Stage;
	config.targetTag = static_cast<uint32_t>(CollTag::Unit);
	SetColliderConfig(config);

	// 当たり判定の初期化
	Initialize();

#ifdef USE_IMGUI
	// 値の登録
	RegisterDebugParam();
#endif
	// 値の適応
	ApplyDebugParam();
}

void GoldOre::Update() {
#ifdef USE_IMGUI
	// 値の適応
	ApplyDebugParam();
#endif

	contactNum_ = 0;

	// 更新処理
	object_->Update();

	// 当たり判定の位置を更新
	quadCollider_.topLeft = { -size_.x * 0.5f + object_->transform_.position.x, -size_.z * 0.5f + object_->transform_.position.z };
	quadCollider_.bottomRight = { size_.x * 0.5f + object_->transform_.position.x, size_.z * 0.5f + object_->transform_.position.z };
}

void GoldOre::Draw(Window* window, const Matrix4x4& vpMatrix) {
	// 描画
	object_->Draw(window, vpMatrix);
}

void GoldOre::OnCollision(Collider* other) {

	bool isUnit = CollTag::Unit == other->GetOwnTag();

	if (!isUnit) { return; }

	// ユニットとの接触回数を記録
	contactNum_++;
}

void GoldOre::RegisterDebugParam() {

}

void GoldOre::ApplyDebugParam() {

}