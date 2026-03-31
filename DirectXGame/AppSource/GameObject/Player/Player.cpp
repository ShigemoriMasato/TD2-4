#include "Player.h"
#include <../Engine/Assets/Audio/AudioManager.h>
#include <GameObject/Enemy/IEnemy.h>
#include <Utility/Matrix.h>
#include <Utility/MatrixFactory.h>
#include <imgui/imgui.h>

using namespace SHEngine;
using namespace Player;

void Base::Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager, CharacterID characterID, ItemManager* itemManager) {
	for (int i = 0; i < static_cast<int>(PartIndex::Count); ++i) {
		// 本体描画用オブジェクトの生成&初期化
		render_[i] = std::make_unique<RenderObject>("Player");
		render_[i]->Initialize();

		// シェーダーの設定
		render_[i]->psoConfig_.vs = "Game/Field.VS.hlsl";
		render_[i]->psoConfig_.ps = "Game/Field.PS.hlsl";
		render_[i]->SetUseTexture(true);

		// 描画するデータの読み込み
		int modelHandle = modelManager->LoadModel("Assets/Model/player/player-" + std::to_string(i));
		auto modelData = modelManager->GetNodeModelData(modelHandle);
		auto drawData = drawDataManager->GetDrawData(modelData.drawDataIndex);
		render_[i]->SetDrawData(drawData);

		// テクスチャインデックスを保存
		auto& material = modelData.materials[modelData.materialIndex.front()];
		textureIndex_ = material.textureIndex;

		// CBVの設定
		render_[i]->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER);
		render_[i]->CreateCBV(sizeof(Vector4), ShaderType::PIXEL_SHADER, "Color");
		render_[i]->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "TextureIndex");
	}

	// 単位行列の代入
	wvp_ = Matrix4x4::Identity();

	// 状態の初期化
	currentState_ = std::make_unique<StateNormal>(); // 通常

	// Transformの初期化
	transform_.position = {19.0f, 0.0f, 19.0f};

	collCircle_ = std::make_unique<Circle>();
	collCircle_->center = {transform_.position.x, transform_.position.z};
	collCircle_->radius = 1.0f; // 仮の半径
	CollConfig collConfig;
	collConfig.ownTag = CollTag::Player;
	collConfig.targetTag = CollTag::Enemy | CollTag::Item;
	collConfig.colliderInfo = collCircle_.get();
	collConfig.isActive = true;
	Collider::Initialize();
	SetColliderConfig(collConfig);

	// パラメータリストの生成&初期化
	parameterList_ = std::make_unique<ParameterList>();
	parameterList_->Initialize(itemManager);

	logger_ = getLogger("Player");

	// HPの初期化
	// maxHP_ = parameterList_->GetParameter("MaxHP");
	maxHP_ = 500; // 仮の値
	currentHP_ = maxHP_;

	// 各パーツのローカルTransformの初期化
	for (auto& pTransform : partTransforms_) {
		pTransform.scale = {1.0f, 1.0f, 1.0f};
		pTransform.rotate = {0.0f, 0.0f, 0.0f};
	}

	partTransforms_[static_cast<int>(PartIndex::Body)].position = {0.0f, 2.8f, 0.0f};
	partTransforms_[static_cast<int>(PartIndex::RightArm)].position = {-0.6f, 1.7f, 0.0f};
	partTransforms_[static_cast<int>(PartIndex::LeftArm)].position = {0.6f, 1.7f, 0.0f};
	partTransforms_[static_cast<int>(PartIndex::RightLeg)].position = {-0.25f, 0.4f, 0.0f};
	partTransforms_[static_cast<int>(PartIndex::LeftLeg)].position = {0.25f, 0.4f, 0.0f};
}

void Base::Update(Matrix4x4 vpMatrix, float deltaTime, std::unordered_map<Key, bool>& key) {
	// 描画用にVP行列を保存
	vpMatrix_ = vpMatrix;

	// 現在の状態の更新処理
	if (currentState_) {
		currentState_->Update(this, deltaTime);
	}

	maxHP_ = std::clamp(maxHP_, lowerLimitHP_, upperLimitHP_);
	currentHP_ = std::clamp(currentHP_, 0.0f, maxHP_);

#ifdef _DEBUG
	// HP
	if (key[Key::Damage]) {
		Damage(1.0f);
	}
	if (key[Key::Heal]) {
		Heal(1.0f);
	}
	if (key[Key::FullHeal]) {
		currentHP_ = maxHP_;
	}
	if (key[Key::FullDamage]) {
		currentHP_ = 0.0f;
	}
	if (key[Key::InvincibleChange]) {
		isDebugInvincible_ = !isDebugInvincible_;
	}
#endif

	// プレイヤーの移動制限
	ClampPosition();

	// 座標を行列に変換
	wvp_ = Matrix::MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.position);
	wvp_ *= vpMatrix;

	// プレイヤー本体のワールド行列を計算
	Matrix4x4 matWorldBody = Matrix::MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.position);

	// 各パーツの関節位置のオフセット
	Vector3 pivotOffset[static_cast<int>(PartIndex::Count)] = {
	    {0.0f, 0.8f, 0.0f}, // RightArm
	    {0.0f, 0.8f, 0.0f}, // LeftArm
	    {0.0f, 0.8f, 0.0f}, // RightLeg
	    {0.0f, 0.8f, 0.0f}, // LeftLeg
	    {0.0f, 0.0f, 0.0f}  // Body
	};

	for (int i = 0; i < static_cast<int>(PartIndex::Count); ++i) {
		// モデルの中心を原点から関節位置へずらす行列
		Matrix4x4 matOffset = Matrix::MakeAffineMatrix({1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {-pivotOffset[i].x, -pivotOffset[i].y, -pivotOffset[i].z});

		// 本来の中心座標に関節オフセットを足して、関節自体のローカル座標を求める
		Vector3 jointPos = {partTransforms_[i].position.x + pivotOffset[i].x, partTransforms_[i].position.y + pivotOffset[i].y, partTransforms_[i].position.z + pivotOffset[i].z};

		// スケール・回転・関節座標への移動を行う行列
		Matrix4x4 matSRT = Matrix::MakeAffineMatrix(partTransforms_[i].scale, partTransforms_[i].rotate, jointPos);

		// オフセット行列と合成
		Matrix4x4 matLocal = matOffset * matSRT;
		Matrix4x4 matWorldPart = matLocal * matWorldBody;

		// WVP行列を計算
		Matrix4x4 wvp = matWorldPart * vpMatrix;

		Vector4 color = {1.0f, 1.0f, 1.0f, 1.0f};

		render_[i]->CopyBufferData(0, &wvp, sizeof(Matrix4x4));
		render_[i]->CopyBufferData(1, &color, sizeof(Vector4));
		render_[i]->CopyBufferData(2, &textureIndex_, sizeof(int));
	}

	collCircle_->center = {transform_.position.x, transform_.position.z};

	// 無敵時間更新
	if (isInvincible_) {
		invincibleTimer_ -= deltaTime;
		if (invincibleTimer_ <= 0.0f) {
			isInvincible_ = false;
			invincibleTimer_ = 0.0f;
		}
	}
}

void Player::Base::UpdateParameter(const std::vector<Piece*>& items) { parameterList_->Update(items); }

void Base::Draw(CmdObj* cmdObj) {
	for (int i = 0; i < static_cast<int>(PartIndex::Count); ++i) {
		// 本体の描画
		render_[i]->Draw(cmdObj);

#ifdef USE_IMGUI
		ImGui::Begin("Param");

		std::string labelScale = "Scale##" + std::to_string(i);
		std::string labelRotate = "Rotate##" + std::to_string(i);
		std::string labelPos = "Position##" + std::to_string(i);

		ImGui::DragFloat3(labelScale.c_str(), &partTransforms_[i].scale.x, 0.01f);
		ImGui::DragFloat3(labelRotate.c_str(), &partTransforms_[i].rotate.x, 0.01f);
		ImGui::DragFloat3(labelPos.c_str(), &partTransforms_[i].position.x, 0.01f);

		ImGui::End();
#endif
	}
}

void Base::ChangeState(std::unique_ptr<IPlayerState> newState) {
	if (currentState_) {
		currentState_->Exit(this); // 古い状態の終了処理
	}

	// 状態の切り替え
	currentState_ = std::move(newState);

	if (currentState_) {
		currentState_->Enter(this); // 新しい状態の開始処理
	}
}

void Player::Base::OnCollision(Collider* other) {
	if (other->GetOwnTag() & CollTag::Enemy) {
		auto enemy = static_cast<IEnemy*>(other);
		Damage(enemy->GetAttack());
	} else {
		Damage(1.0f);
	}
}

void Player::Base::Damage(float amount) {
#ifdef _DEBUG
	if (isDebugInvincible_)
		return;
#endif

	if (amount <= 0.0f || isInvincible_)
		return;

	currentHP_ = std::max(currentHP_ - amount, 0.0f);
	isInvincible_ = true;
	invincibleTimer_ = invincibleDuration_; // タイマーをセット

	// SE再生
	uint32_t handle = AudioManager::GetInstance().GetHandleByName("PlayerDamage.mp3");
	if(handle != 0){
		AudioManager::GetInstance().Play(handle, 0.1f, false);
	}
}

void Player::Base::Heal(float amount) {
	if (amount <= 0.0f)
		return;
	currentHP_ = std::min(currentHP_ + amount, maxHP_);
}

void Player::Base::UpdateWalkAnimation(float deltaTime, bool isMoving) {
	// 腕と脚を振る最大角度と片道の時間
	const float maxAngle = std::numbers::pi_v<float> / 4.0f;
	const float animDuration = 0.5f;

	if (isMoving) {
		// 停止状態から移動状態に切り替わった瞬間
		if (!wasMoving_) {
			// 現在の角度から、歩き始めの目標角度へアニメーションを開始
			rotateAnimationRightArm_.anim.Start(rotateAnimationRightArm_.temp, {maxAngle, 0, 0}, animDuration / 2.0f, EaseType::EaseInOutSine);
			rotateAnimationLeftArm_.anim.Start(rotateAnimationLeftArm_.temp, {-maxAngle, 0, 0}, animDuration / 2.0f, EaseType::EaseInOutSine);
			rotateAnimationRightLeg_.anim.Start(rotateAnimationRightLeg_.temp, {-maxAngle, 0, 0}, animDuration / 2.0f, EaseType::EaseInOutSine);
			rotateAnimationLeftLeg_.anim.Start(rotateAnimationLeftLeg_.temp, {maxAngle, 0, 0}, animDuration / 2.0f, EaseType::EaseInOutSine);
		} else {
			// アニメーションの更新
			bool isPlayingRA = rotateAnimationRightArm_.anim.Update(deltaTime, rotateAnimationRightArm_.temp);
			rotateAnimationLeftArm_.anim.Update(deltaTime, rotateAnimationLeftArm_.temp);
			rotateAnimationRightLeg_.anim.Update(deltaTime, rotateAnimationRightLeg_.temp);
			rotateAnimationLeftLeg_.anim.Update(deltaTime, rotateAnimationLeftLeg_.temp);

			// 片道のアニメーションが終了したら、反転させてループ
			if (!isPlayingRA) {
				if (rotateAnimationRightArm_.temp.x > 0.0f) {
					// 前から後ろ
					rotateAnimationRightArm_.anim.Start({maxAngle, 0, 0}, {-maxAngle, 0, 0}, animDuration, EaseType::EaseInOutSine);
					rotateAnimationLeftArm_.anim.Start({-maxAngle, 0, 0}, {maxAngle, 0, 0}, animDuration, EaseType::EaseInOutSine);
					rotateAnimationRightLeg_.anim.Start({-maxAngle, 0, 0}, {maxAngle, 0, 0}, animDuration, EaseType::EaseInOutSine);
					rotateAnimationLeftLeg_.anim.Start({maxAngle, 0, 0}, {-maxAngle, 0, 0}, animDuration, EaseType::EaseInOutSine);
				} else {
					// 後ろから前
					rotateAnimationRightArm_.anim.Start({-maxAngle, 0, 0}, {maxAngle, 0, 0}, animDuration, EaseType::EaseInOutSine);
					rotateAnimationLeftArm_.anim.Start({maxAngle, 0, 0}, {-maxAngle, 0, 0}, animDuration, EaseType::EaseInOutSine);
					rotateAnimationRightLeg_.anim.Start({maxAngle, 0, 0}, {-maxAngle, 0, 0}, animDuration, EaseType::EaseInOutSine);
					rotateAnimationLeftLeg_.anim.Start({-maxAngle, 0, 0}, {maxAngle, 0, 0}, animDuration, EaseType::EaseInOutSine);
				}
			}
		}
	} else {
		// 移動状態から停止状態に切り替わった瞬間
		if (wasMoving_) {
			// 現在の途中の角度から、気をつけの姿勢に戻るアニメーションを開始
			float returnDuration = 0.25f;
			rotateAnimationRightArm_.anim.Start(rotateAnimationRightArm_.temp, {0.0f, 0.0f, 0.0f}, returnDuration, EaseType::EaseOutSine);
			rotateAnimationLeftArm_.anim.Start(rotateAnimationLeftArm_.temp, {0.0f, 0.0f, 0.0f}, returnDuration, EaseType::EaseOutSine);
			rotateAnimationRightLeg_.anim.Start(rotateAnimationRightLeg_.temp, {0.0f, 0.0f, 0.0f}, returnDuration, EaseType::EaseOutSine);
			rotateAnimationLeftLeg_.anim.Start(rotateAnimationLeftLeg_.temp, {0.0f, 0.0f, 0.0f}, returnDuration, EaseType::EaseOutSine);
		}

		// 戻りアニメーションの更新
		rotateAnimationRightArm_.anim.Update(deltaTime, rotateAnimationRightArm_.temp);
		rotateAnimationLeftArm_.anim.Update(deltaTime, rotateAnimationLeftArm_.temp);
		rotateAnimationRightLeg_.anim.Update(deltaTime, rotateAnimationRightLeg_.temp);
		rotateAnimationLeftLeg_.anim.Update(deltaTime, rotateAnimationLeftLeg_.temp);
	}

	// 次のフレームの検知用に状態を保存
	wasMoving_ = isMoving;

	// 各パーツのローカル回転にアニメーションの計算結果を代入
	partTransforms_[static_cast<int>(PartIndex::RightArm)].rotate = rotateAnimationRightArm_.temp;
	partTransforms_[static_cast<int>(PartIndex::LeftArm)].rotate = rotateAnimationLeftArm_.temp;
	partTransforms_[static_cast<int>(PartIndex::RightLeg)].rotate = rotateAnimationRightLeg_.temp;
	partTransforms_[static_cast<int>(PartIndex::LeftLeg)].rotate = rotateAnimationLeftLeg_.temp;
}

void Player::Base::ClampPosition() {
	// プレイヤーがステージ買いに出ないようにする
	float posX = std::clamp(transform_.position.x, mapInfo_.minX, mapInfo_.maxX);
	float posZ = std::clamp(transform_.position.z, mapInfo_.minZ, mapInfo_.maxZ);

	transform_.position = Vector3(posX, 0.0f, posZ);
}

float Base::GetParameter(const std::string& paramName) const {
	auto param = parameterList_->GetAllParameters();
	auto it = param.find(paramName);
	if (it != param.end()) {
		return it->second;
	}

	// 間違っている場合ログを残す
	logger_->warn("Parameter '{}' not found. Returning 0.", paramName);
	return 0.0f; // パラメータが見つからない場合は0を返す
}