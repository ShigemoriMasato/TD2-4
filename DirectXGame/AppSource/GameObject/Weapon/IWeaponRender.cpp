#include "IWeaponRender.h"
#include <numbers>

using namespace SHEngine;

void IWeaponRender::Initialize(SHEngine::DrawDataManager* drawDataManager, SHEngine::ModelManager* modelManager, SHEngine::TextureManager* textureManager, IWeapon* weapon, Item itemData) {
	render_ = std::make_unique<RenderObject>();
	weapon_ = weapon;

	int modelHandle = itemData.modelID;
	auto modelData = modelManager->GetNodeModelData(modelHandle);
	auto drawData = drawDataManager->GetDrawData(modelData.drawDataIndex);

	auto& material = modelData.materials[modelData.materialIndex.front()];
	textureIndex_ = material.textureIndex;

	// モデルを初期化
	render_ = std::make_unique<RenderObject>();
	render_->Initialize();

	// シェーダー設定
	render_->psoConfig_.vs = "Game/Field.VS.hlsl";
	render_->psoConfig_.ps = "Game/Field.PS.hlsl";
	render_->SetUseTexture(true);

	// CBVの生成
	render_->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER);
	render_->CreateCBV(sizeof(Vector4), ShaderType::PIXEL_SHADER, "Color");
	render_->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "TextureIndex");

	// 描画データを設定
	render_->SetDrawData(drawData);

	// ワールド座標を設定
	transform_.position = {0.0f, 0.0f, 0.0f};
	transform_.rotate = {0.0f, 0.0f, -std::numbers::pi_v<float> / 2.0f};
	transform_.scale = {0.5f, 0.5f, 0.5f};

	// 単位行列の代入
	wvp_ = Matrix4x4::Identity();

	// トレイル
	WeaponData* wData = weapon_->GetWeaponData();
	trailSword_.Initialize(drawDataManager, textureManager, &trailDataBank_);
	trailSpear_.Initialize(drawDataManager, textureManager, &trailDataBank_);
	trailSword_.Add("Sword_Ribbon");
	trailSpear_.Add("Spear_Ribbon");
}

void IWeaponRender::Update(Matrix4x4 vpMatrix, Vector3 playerPos, float deltaTime) {
	WeaponData* wData = weapon_->GetWeaponData();
	bool currentIsAnimation = weapon_->GetIsAnimation();

	if (currentIsAnimation && !prevIsAnimation_) {
		animState_ = AnimState::Forward;
		direction_ = weapon_->GetDirection();

		// アニメーションの初期値と目標値
		Vector3 posStart = {0.0f, 0.0f, 0.0f};
		Vector3 posEnd = {0.0f, 0.0f, 0.0f};
		Vector3 rotStart = {0.0f, 0.0f, 0.0f};
		Vector3 rotEnd = {0.0f, 0.0f, 0.0f};
		float forwardDuration = 0.2f;

		switch (wData->type) {
		case WeaponType::Pistol: {
			forwardDuration = 0.05f;
			float recoilAngle = 0.5f;
			rotEnd = {-std::sinf(direction_) * recoilAngle, 0.0f, std::cosf(direction_) * recoilAngle};
			break;
		}
		case WeaponType::ShotGun: {
			forwardDuration = 0.1f;
			float recoilDist = 1.5f;
			posEnd = {-std::cosf(direction_) * recoilDist, 0.0f, -std::sinf(direction_) * recoilDist};
			break;
		}
		case WeaponType::Sword: {
			forwardDuration = 0.15f;
			rotStart = {0.0f, -std::numbers::pi_v<float> / 4.0f, 0.0f};
			rotEnd = {0.0f, std::numbers::pi_v<float> / 4.0f, 0.0f};
			rotOffsetAnim_.temp = rotStart;
			break;
		}
		case WeaponType::Spear: {
			forwardDuration = 0.1f;
			float thrustDistance = 3.0f;
			posEnd = {std::cosf(direction_) * thrustDistance, 0.0f, std::sinf(direction_) * thrustDistance};
			break;
		}
		}

		// 座標と回転のアニメーション開始
		posOffsetAnim_.anim.Start(posStart, posEnd, forwardDuration, EaseType::EaseOutCubic);
		rotOffsetAnim_.anim.Start(rotStart, rotEnd, forwardDuration, EaseType::EaseOutCubic);

		weapon_->SetIsAnimation(false); // アニメーションフラグを下す
	}
	prevIsAnimation_ = currentIsAnimation; // 次フレームのために状態を保存

	if (animState_ == AnimState::Forward) {
		bool posPlaying = posOffsetAnim_.anim.Update(deltaTime, posOffsetAnim_.temp);
		bool rotPlaying = rotOffsetAnim_.anim.Update(deltaTime, rotOffsetAnim_.temp);

		// 位置と回転、両方のアニメーションが終わったら戻りアニメーションへ遷移
		if (!posPlaying && !rotPlaying) {
			animState_ = AnimState::Return;

			// 武器ごとに戻るスピードを調整
			float returnDuration = 0.2f;
			switch (wData->type) {
			case WeaponType::Pistol:
				returnDuration = 0.2f;
				break;
			case WeaponType::ShotGun:
				returnDuration = 0.3f;
				break;
			case WeaponType::Sword:
				returnDuration = 0.25f;
				break;
			case WeaponType::Spear:
				returnDuration = 0.3f;
				break;
			}

			// アニメーションの開始
			posOffsetAnim_.anim.Start(posOffsetAnim_.temp, {0.0f, 0.0f, 0.0f}, returnDuration, EaseType::EaseOutCubic);
			rotOffsetAnim_.anim.Start(rotOffsetAnim_.temp, {0.0f, 0.0f, 0.0f}, returnDuration, EaseType::EaseOutCubic);
		}
	} else if (animState_ == AnimState::Return) {
		bool posPlaying = posOffsetAnim_.anim.Update(deltaTime, posOffsetAnim_.temp);
		bool rotPlaying = rotOffsetAnim_.anim.Update(deltaTime, rotOffsetAnim_.temp);

		// 戻りアニメーションの終了
		if (!posPlaying && !rotPlaying) {
			animState_ = AnimState::None;
			posOffsetAnim_.temp = {0.0f, 0.0f, 0.0f};
			rotOffsetAnim_.temp = {0.0f, 0.0f, 0.0f};
		}
	}

	float currentDir = direction_;

	if (animState_ != AnimState::None && wData->type == WeaponType::Sword) {
		currentDir += rotOffsetAnim_.temp.y;
	}

	// プレイヤーの周りに武器が配置されるようにするための処理
	transform_.position.x = std::cosf(currentDir) * 4.0f;
	transform_.position.z = std::sinf(currentDir) * 4.0f;
	transform_.position.y = 3.0f;
	transform_.position += playerPos;

	// 回転はDirectionを向かせる
	transform_.rotate = {currentDir - std::numbers::pi_v<float> / 2, 0.0f, -std::numbers::pi_v<float> / 2.0f};

	// アニメーション実行中であれば算出したオフセットを加算
	if (animState_ != AnimState::None) {
		transform_.position.x += posOffsetAnim_.temp.x;
		transform_.position.y += posOffsetAnim_.temp.y;
		transform_.position.z += posOffsetAnim_.temp.z;

		if (wData->type != WeaponType::Sword) {
			transform_.rotate.x += rotOffsetAnim_.temp.x;
			transform_.rotate.y += rotOffsetAnim_.temp.y;
			transform_.rotate.z += rotOffsetAnim_.temp.z;
		}
	}

	wvp_ = Matrix::MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.position);
	trailSpear_.SetModelWorld(wvp_);
	trailSpear_.Update(deltaTime, vpMatrix);
	trailSword_.SetModelWorld(wvp_);
	trailSword_.Update(deltaTime, vpMatrix);
	wvp_ *= vpMatrix;
	Vector4 color = {1.0f, 1.0f, 1.0f, 1.0f};
	render_->CopyBufferData(0, &wvp_, sizeof(Matrix4x4));
	render_->CopyBufferData(1, &color, sizeof(Vector4));
	render_->CopyBufferData(2, &textureIndex_, sizeof(int));
}

void IWeaponRender::Draw(CmdObj* cmdObj) {
	render_->Draw(cmdObj);

	WeaponData* wData = weapon_->GetWeaponData();
	switch (wData->type) {
	case WeaponType::Sword:
		trailSword_.Draw(cmdObj);
		break;
	case WeaponType::Spear:
		trailSpear_.Draw(cmdObj);
		break;
	default:
		break;
	}
}

Matrix4x4 IWeaponRender::LookAt(const Vector3& direction, const Vector3& up) {
	Vector3 forward = MyMath::Normalize(direction);
	Vector3 right = MyMath::Normalize(MyMath::cross(up, forward));
	Vector3 trueUp = MyMath::cross(forward, right);

	Matrix4x4 m;

	m.m[0][0] = right.x;
	m.m[1][0] = right.y;
	m.m[2][0] = right.z;

	m.m[0][1] = trueUp.x;
	m.m[1][1] = trueUp.y;
	m.m[2][1] = trueUp.z;

	m.m[0][2] = forward.x;
	m.m[1][2] = forward.y;
	m.m[2][2] = forward.z;

	m.m[3][3] = 1;

	return m;
}