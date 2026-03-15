#include "Player.h"
#include <Utility/Matrix.h>
#include <Utility/MatrixFactory.h>
#include <imgui/imgui.h>

using namespace SHEngine;
using namespace Player;

void Base::Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager, Input* input, CharacterID characterID, ItemManager* itemManager) {
	// 本体描画用オブジェクトの生成&初期化
	render_ = std::make_unique<RenderObject>();
	render_->Initialize();

	// シェーダーの設定
	render_->psoConfig_.vs = "Game/Field.VS.hlsl";
	render_->psoConfig_.ps = "Game/Field.PS.hlsl";
	render_->SetUseTexture(true);

	// 描画するデータの読み込み
	int modelHandle = modelManager->LoadModel("Assets/Model/player");
	auto modelData = modelManager->GetNodeModelData(modelHandle);
	auto drawData = drawDataManager->GetDrawData(modelData.drawDataIndex);
	render_->SetDrawData(drawData);

	// テクスチャインデックスを保存
	auto& material = modelData.materials[modelData.materialIndex.front()];
	textureIndex_ = material.textureIndex;

	// CBVの設定
	render_->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER);
	render_->CreateCBV(sizeof(Vector4), ShaderType::PIXEL_SHADER, "Color");
	render_->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "TextureIndex");

	// 残像描画用オブジェクトの生成&初期化
	afterImageRender_ = std::make_unique<RenderObject>();
	afterImageRender_->Initialize();

	// シェーダーの設定
	afterImageRender_->psoConfig_.vs = "Game/AfterImage.VS.hlsl";
	afterImageRender_->psoConfig_.ps = "Game/AfterImage.PS.hlsl";
	afterImageRender_->SetUseTexture(true);
	afterImageRender_->SetDrawData(drawData);

	// CBVの設定
	afterImageRender_->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER, "WorldMatrix");

	// SRVの設定
	afterImageRender_->CreateSRV(sizeof(Matrix4x4), kMaxInstanceAfterImage, ShaderType::VERTEX_SHADER, "ViwProj");
	afterImageRender_->CreateSRV(sizeof(Vector4), kMaxInstanceAfterImage, ShaderType::PIXEL_SHADER, "Colors");

	// 単位行列の代入
	wvp_ = Matrix4x4::Identity();

	// 入力
	input_ = input;

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
	//maxHP_ = parameterList_->GetParameter("MaxHP");
	maxHP_ = 10; // 仮の値
	currentHP_ = maxHP_;
}

void Base::Update(Matrix4x4 vpMatrix, float deltaTime) {
	// 描画用にVP行列を保存
	vpMatrix_ = vpMatrix;

	// 現在の状態の更新処理
	if (currentState_) {
		currentState_->Update(this, deltaTime);
	}

#ifdef _DEBUG
	// HP
	if (input_->GetKeyState(DIK_1) && !input_->GetPreKeyState(DIK_1)) {
		Damage(1.0f);
	}
	if (input_->GetKeyState(DIK_2) && !input_->GetPreKeyState(DIK_2)) {
		Heal(1.0f);
	}
	if (input_->GetKeyState(DIK_3) && !input_->GetPreKeyState(DIK_3)) {
		currentHP_ = maxHP_;
	}
	if (input_->GetKeyState(DIK_4) && !input_->GetPreKeyState(DIK_4)) {
		currentHP_ = 0.0f;
	}
#endif

	// 残像の更新
	UpdateAfterImages(deltaTime);

	// プレイヤーの移動制限
	ClampPosition();

	// 座標を行列に変換
	wvp_ = Matrix::MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.position);
	wvp_ *= vpMatrix;

	// wvp行列を描画に適用
	render_->CopyBufferData(0, &wvp_, sizeof(Matrix4x4));

	// 色の指定
	Vector4 color = {1.0f, 1.0f, 1.0f, 1.0f};
	render_->CopyBufferData(1, &color, sizeof(Vector4));

	// テクスチャ
	render_->CopyBufferData(2, &textureIndex_, sizeof(int));

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
	// 残像の描画
	if (!afterImages_.empty()) {
		std::vector<Matrix4x4> worldMatrices;
		std::vector<Vector4> colors;

		for (const auto& ai : afterImages_) {
			// Transformからワールド行列を計算
			Matrix4x4 world = Matrix::MakeAffineMatrix(ai.transform.scale, ai.transform.rotate, ai.transform.position);
			worldMatrices.push_back(world);

			// 寿命に応じて透明度を下げる
			float alpha = ai.timer / afterImageLifeTime_;

			colors.push_back(Vector4(1.0f, 1.0f, 1.0f, alpha));
		}

		afterImageRender_->CopyBufferData(0, &vpMatrix_, sizeof(Matrix4x4));
		afterImageRender_->CopyBufferData(1, worldMatrices.data(), sizeof(Matrix4x4) * worldMatrices.size());
		afterImageRender_->CopyBufferData(2, colors.data(), sizeof(Vector4) * colors.size());

		afterImageRender_->instanceNum_ = static_cast<int>(worldMatrices.size());

		// 描画
		afterImageRender_->Draw(cmdObj);
	}

	// 本体の描画
	render_->Draw(cmdObj);

#ifdef USE_IMGUI
#endif // USE_IMGUI
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

void Player::Base::SpawnAfterImage() {
	AfterImage ai;
	ai.transform = transform_;      // 現在のプレイヤーの姿勢を代入
	ai.timer = afterImageLifeTime_; // 寿命をセット
	afterImages_.push_back(ai);
}

void Player::Base::OnCollision(Collider* other) {
	Damage(1.0f);
}

void Player::Base::Damage(float amount){
	if (amount <= 0.0f || isInvincible_)
		return;

	currentHP_ = std::max(currentHP_ - amount, 0.0f);
	isInvincible_ = true;
	invincibleTimer_ = invincibleDuration_; // タイマーをセット
}

void Player::Base::Heal(float amount) {
	if (amount <= 0.0f)
		return;
	currentHP_ = std::min(currentHP_ + amount, maxHP_);
}

void Player::Base::UpdateAfterImages(float deltaTime) {
	// 残像のタイマーを減らし0以下になったらリストから削除
	for (auto it = afterImages_.begin(); it != afterImages_.end();) {
		it->timer -= deltaTime;
		if (it->timer <= 0.0f) {
			it = afterImages_.erase(it); // 削除
		} else {
			++it;
		}
	}
}

void Player::Base::ClampPosition() {
	// プレイヤーがステージ買いに出ないようにする
	float posX = std::clamp(transform_.position.x, minX_, maxX_);
	float posZ = std::clamp(transform_.position.z, minZ_, maxZ_);

	transform_.position = Vector3(posX, 0.0f, posZ);
}

void Base::UpdateDashCooldown(float deltaTime) {
	// ダッシュタイマーの減算
	if (dashCooldownTimer_ > 0.0f) {
		dashCooldownTimer_ -= deltaTime;
	}
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