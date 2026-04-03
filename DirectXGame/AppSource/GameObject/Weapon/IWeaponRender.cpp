#include "IWeaponRender.h"
#include <../Engine/Assets/Audio/AudioManager.h>
#include <numbers>

using namespace SHEngine;

void IWeaponRender::Initialize(
    SHEngine::DrawDataManager* drawDataManager, SHEngine::ModelManager* modelManager, SHEngine::TextureManager* textureManager, IWeapon* weapon, Item itemData, const std::string& trailname) {
	render_ = std::make_unique<RenderObject>();
	weapon_ = weapon;

	int modelHandle = itemData.modelID;
	auto modelData = modelManager->GetNodeModelData(modelHandle);
	auto drawData = drawDataManager->GetDrawData(modelData.drawDataIndex);

	auto& material = modelData.materials[modelData.materialIndex.front()];
	textureIndex_ = material.textureIndex;

	// モデルを初期化
	render_ = std::make_unique<RenderObject>("IWeaponRender");
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

	if (trailname.size() != 0) {
		trail_.Initialize(drawDataManager, textureManager, &trailDataBank_);
		trail_.Add(trailname);
	}
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
		Vector3 scaleStart = {0.5f, 0.5f, 0.5f};
		Vector3 scaleEnd = {0.5f, 0.5f, 0.5f};
		float forwardDuration = 0.2f;

		switch (wData->type) {
		case WeaponType::Pistol: {
			forwardDuration = 0.05f;
			float recoilAngle = 0.5f;
			rotEnd = {0.0f, recoilAngle, 0.0f};
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
		case WeaponType::Axe: {
			forwardDuration = 0.15f;
			rotStart = {0.0f, -std::numbers::pi_v<float> / 4.0f, 0.0f};
			rotEnd = {0.0f, std::numbers::pi_v<float> / 4.0f, 0.0f};
			rotOffsetAnim_.temp = rotStart;
			break;
		}
		case WeaponType::Fist: {
			forwardDuration = 0.1f;
			float thrustDistance = 2.0f;
			posEnd = {std::cosf(direction_) * thrustDistance, 0.0f, std::sinf(direction_) * thrustDistance};
			break;
		}
		case WeaponType::Bow: {
			forwardDuration = 0.15f;
			scaleStart = {0.5f, 0.5f, 0.5f};
			scaleEnd = {0.6f, 0.6f, 0.6f};
			break;
		}
		case WeaponType::Gurepon: {
			forwardDuration = 0.05f;
			float recoilAngle = 0.5f;
			rotEnd = {0.0f, direction_ * recoilAngle, 0.0f};
			rotOffsetAnim_.temp = {-(direction_ - std::numbers::pi_v<float> / 2.0f), 0.0f, std::numbers::pi_v<float> / 2.0f};
			break;
		}
		case WeaponType::Pickaxe: {
			forwardDuration = 0.15f;
			currentAnimIsThrust_ = isPickaxeThrust_; // 実行中の状態を保存

			if (currentAnimIsThrust_) {
				// 突き
				float thrustDistance = 3.0f;
				posEnd = {std::cosf(direction_) * thrustDistance, 0.0f, std::sinf(direction_) * thrustDistance};
			} else {
				// 薙ぎ払い
				rotStart = {0.0f, -std::numbers::pi_v<float> / 4.0f, 0.0f};
				rotEnd = {0.0f, std::numbers::pi_v<float> / 4.0f, 0.0f};
				rotOffsetAnim_.temp = rotStart;
			}
			isPickaxeThrust_ = !isPickaxeThrust_; // 次回のアニメーション用に反転させる
			break;
		}
		}

		// SE再生
		{
			std::string seName;
			switch (wData->type) {
			case WeaponType::Pistol:
				seName = "Pistol.mp3";
				break;
			case WeaponType::ShotGun:
				seName = "ShotGun.mp3";
				break;
			case WeaponType::Sword:
				seName = "Sword.mp3";
				break;
			case WeaponType::Spear:
				seName = "Spear.mp3";
				break;
			case WeaponType::Axe:
				seName = "Axe.mp3";
				break;
			case WeaponType::Fist:
				seName = "Fist.mp3";
				break;
			case WeaponType::Bow:
				seName = "Bow.mp3";
				break;
			case WeaponType::Gurepon:
				seName = "GureponShot.mp3";
				break;
			case WeaponType::Pickaxe:
				if (currentAnimIsThrust_) {
					seName = "Spear.mp3";
				} else {
					seName = "Sword.mp3";
				}
				break;
			default:
				seName.clear();
				break;
			}

			if (!seName.empty()) {
				uint32_t handle = AudioManager::GetInstance().GetHandleByName(seName);
				if (handle != 0) {
					AudioManager::GetInstance().Play(handle, 0.1f, false);
				}
			}
		}

		// 座標と回転のアニメーション開始
		posOffsetAnim_.anim.Start(posStart, posEnd, forwardDuration, EaseType::EaseOutCubic);
		rotOffsetAnim_.anim.Start(rotStart, rotEnd, forwardDuration, EaseType::EaseOutCubic);
		scaleOffsetAnim_.anim.Start(scaleStart, scaleEnd, forwardDuration, EaseType::EaseOutCubic);

		weapon_->SetIsAnimation(false); // アニメーションフラグを下す
	}
	prevIsAnimation_ = currentIsAnimation; // 次フレームのために状態を保存

	if (animState_ == AnimState::Forward) {
		bool posPlaying = posOffsetAnim_.anim.Update(deltaTime, posOffsetAnim_.temp);
		bool rotPlaying = rotOffsetAnim_.anim.Update(deltaTime, rotOffsetAnim_.temp);
		bool scalePlaying = scaleOffsetAnim_.anim.Update(deltaTime, scaleOffsetAnim_.temp);

		// 位置と回転、両方のアニメーションが終わったら戻りアニメーションへ遷移
		if (!posPlaying && !rotPlaying && !scalePlaying) {
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
			case WeaponType::Axe:
				returnDuration = 0.3f;
				break;
			case WeaponType::Fist:
				returnDuration = 0.3f;
				break;
			case WeaponType::Bow:
				returnDuration = 0.3f;
				break;
			case WeaponType::Gurepon:
				returnDuration = 0.3f;
				break;
			case WeaponType::Pickaxe:
				returnDuration = 0.3f;
				break;
			}

			// アニメーションの開始
			posOffsetAnim_.anim.Start(posOffsetAnim_.temp, {0.0f, 0.0f, 0.0f}, returnDuration, EaseType::EaseOutCubic);
			rotOffsetAnim_.anim.Start(rotOffsetAnim_.temp, {0.0f, 0.0f, 0.0f}, returnDuration, EaseType::EaseOutCubic);
			scaleOffsetAnim_.anim.Start(scaleOffsetAnim_.temp, {0.5f, 0.5f, 0.5f}, returnDuration, EaseType::EaseOutCubic);
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

	if (animState_ != AnimState::None && (wData->type == WeaponType::Sword || wData->type == WeaponType::Axe || (wData->type == WeaponType::Pickaxe && !currentAnimIsThrust_))) {
		currentDir += rotOffsetAnim_.temp.y;
	}

	// プレイヤーの周りに武器が配置されるようにするための処理
	transform_.position.x = std::cosf(currentDir) * 4.0f;
	transform_.position.z = std::sinf(currentDir) * 4.0f;
	transform_.position.y = 3.0f;
	transform_.position += playerPos;

	// 回転はDirectionを向かせる
	transform_.rotate = {currentDir - std::numbers::pi_v<float> / 2, 0.0f, -std::numbers::pi_v<float> / 2.0f};

	if (wData->type == WeaponType::Pistol || wData->type == WeaponType::ShotGun) {
		// 元の補正を維持
		transform_.rotate.x = currentDir - std::numbers::pi_v<float> / 2.0f;
		transform_.rotate.y = 0.0f;
		transform_.rotate.z = -std::numbers::pi_v<float> / 2.0f;
	} else if (wData->type == WeaponType::Sword || wData->type == WeaponType::Axe || wData->type == WeaponType::Pickaxe) {
		transform_.rotate.x = 0.0f;
		transform_.rotate.y = -(currentDir - std::numbers::pi_v<float> / 2.0f);
		transform_.rotate.z = 0.0f;
	} else if (wData->type == WeaponType::Gurepon) {
		transform_.rotate.x = -(currentDir - std::numbers::pi_v<float> / 2.0f);
		transform_.rotate.y = 0.0f;
		transform_.rotate.z = std::numbers::pi_v<float> / 2.0f;
	}

	// アニメーション実行中であれば算出したオフセットを加算
	if (animState_ != AnimState::None) {
		transform_.scale.x = scaleOffsetAnim_.temp.x;
		transform_.scale.y = scaleOffsetAnim_.temp.y;
		transform_.scale.z = scaleOffsetAnim_.temp.z;

		transform_.position.x += posOffsetAnim_.temp.x;
		transform_.position.y += posOffsetAnim_.temp.y;
		transform_.position.z += posOffsetAnim_.temp.z;

		if (wData->type != WeaponType::Sword && wData->type != WeaponType::Axe) {
			// 傾いているモデルはここで直接回転オフセットを加算
			transform_.rotate.x += rotOffsetAnim_.temp.x;
			transform_.rotate.y += rotOffsetAnim_.temp.y;
			transform_.rotate.z += rotOffsetAnim_.temp.z;
		}
	}

	wvp_ = Matrix::MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.position);
	trail_.SetModelWorld(wvp_);
	trail_.Update(deltaTime, vpMatrix);
	wvp_ *= vpMatrix;
	Vector4 color = {1.0f, 1.0f, 1.0f, 1.0f};
	render_->CopyBufferData(0, &wvp_, sizeof(Matrix4x4));
	render_->CopyBufferData(1, &color, sizeof(Vector4));
	render_->CopyBufferData(2, &textureIndex_, sizeof(int));
}

void IWeaponRender::Draw(CmdObj* cmdObj) {
	render_->Draw(cmdObj);

	if (rotOffsetAnim_.anim.GetIsActive() || posOffsetAnim_.anim.GetIsActive()) {
		trail_.Draw(cmdObj);
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