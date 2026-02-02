#include"GoldOre.h"
#include <Common/DebugParam/GameParamEditor.h>
#include"FpsCount.h"
#include"Unit/Ore/OreUnit.h"

void GoldOre::Init(DrawData drawData, int texture, const Vector3& pos, OreType type, const float& rotY) {
	
	// オブジェクトを初期化
	object_ = std::make_unique<OreObject>();
	object_->Initialize(drawData, texture);

	// タイプ
	type_ = type;

	// 初期位置を設定
	object_->transform_.position = pos;
	object_->transform_.rotate.y = rotY;

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

	// 体力を設定
	hp_ = maxHp_;
}

void GoldOre::Update() {
#ifdef USE_IMGUI
	// 値の適応
	ApplyDebugParam();
#endif

	// 更新処理
	object_->Update();

	// 当たり判定の位置を更新
	quadCollider_.topLeft = { -size_.x * 0.5f + object_->transform_.position.x, -size_.z * 0.5f + object_->transform_.position.z };
	quadCollider_.bottomRight = { size_.x * 0.5f + object_->transform_.position.x, size_.z * 0.5f + object_->transform_.position.z };

	if (isHit_) {
		coolTimer_ += FpsCount::deltaTime / coolTime_;
		if (coolTimer_ >= 1.0f) {
			coolTimer_ = 0.0f;
			isHit_ = false;
		}

		frame_++;
		if (frame_ > 1) {
			isChangeHp_ = false;
		}
	}

	// 体力がないと死亡
	if (hp_ <= 0) {
		isDead_ = true;
	}
}

void GoldOre::Draw(Window* window, const Matrix4x4& vpMatrix) {
	// 描画
	object_->Draw(window, vpMatrix);
}

void GoldOre::OnCollision(Collider* other) {

	bool isUnit = CollTag::Unit == other->GetOwnTag();

	if (isUnit) {

		OreUnit* unit = dynamic_cast<OreUnit*>(other);
		if (unit) {
			if (OreUnit::State::Mining == unit->GetState()) {
				if (!isHit_) {
					frame_ = 0;
					isChangeHp_ = true;
					isHit_ = true;
				}
			}
		}
	} else {
		frame_ = 0;
		coolTimer_ = 0.0f;
		isHit_ = false;
		isChangeHp_ = false;
	}
}

void GoldOre::RegisterDebugParam() {
	// 登録
	GameParamEditor::GetInstance()->AddItem(kGroupName_, "LargeMaxHp", int32_t(10));
	GameParamEditor::GetInstance()->AddItem(kGroupName_, "MediumMaxHp", int32_t(10));
	GameParamEditor::GetInstance()->AddItem(kGroupName_, "SmallMaxHp", int32_t(10));

	GameParamEditor::GetInstance()->AddItem(kGroupName_, "LargeWorkerMaxNum", int32_t(5));
	GameParamEditor::GetInstance()->AddItem(kGroupName_, "MediumWorkerMaxNum", int32_t(5));
	GameParamEditor::GetInstance()->AddItem(kGroupName_, "SmallWorkerMaxNum", int32_t(5));
}

void GoldOre::ApplyDebugParam() {
	// 適応
	switch (type_)
	{
	case OreType::Large:
		maxHp_ = GameParamEditor::GetInstance()->GetValue<int32_t>(kGroupName_, "LargeMaxHp");
		maxWorkerNum_ = GameParamEditor::GetInstance()->GetValue<int32_t>(kGroupName_, "LargeWorkerMaxNum");
		break;

	case OreType::Medium:
		maxHp_ = GameParamEditor::GetInstance()->GetValue<int32_t>(kGroupName_, "MediumMaxHp");
		maxWorkerNum_ = GameParamEditor::GetInstance()->GetValue<int32_t>(kGroupName_, "MediumWorkerMaxNum");
		break;

	case OreType::Small:
		maxHp_ = GameParamEditor::GetInstance()->GetValue<int32_t>(kGroupName_, "SmallMaxHp");
		maxWorkerNum_ = GameParamEditor::GetInstance()->GetValue<int32_t>(kGroupName_, "SmallWorkerMaxNum");
		break;
	}
}