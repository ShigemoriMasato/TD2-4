#include "Player.h"
#include <Utility/Matrix.h>
#include <Utility/MatrixFactory.h>
#include <imgui/imgui.h>

using namespace SHEngine;
using namespace Player;

void Base::Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager, Input* input, CharacterID characterID) {
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

	// ステータスマネージャの生成&初期化
	statusManager_ = std::make_unique<StatusManager>();
	statusManager_->Initialize(characterID);
}

void Base::Update(Matrix4x4 vpMatrix, float deltaTime) {
	// 描画用にVP行列を保存
	vpMatrix_ = vpMatrix;

	// 現在の状態の更新処理
	if (currentState_) {
		currentState_->Update(this, deltaTime);
	}

	// 残像の更新
	UpdateAfterImages(deltaTime);

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
}

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
	ImGui::Begin("Player");

	// --- 基本パラメータ ---
	if (ImGui::CollapsingHeader("Basic Parameters")) {
		ImGui::DragFloat3("scale", &transform_.scale.x, 0.01f);
		ImGui::DragFloat3("rotate", &transform_.rotate.x, 0.01f);
		ImGui::DragFloat3("translate", &transform_.position.x, 0.01f);
		ImGui::DragFloat("velocity", &velocity_, 0.01f);
	}

	// --- ダッシュパラメータ ---
	if (ImGui::CollapsingHeader("Dash Parameters")) {
		ImGui::Text("CanDash : %s", CanDash() ? "True" : "False");
		ImGui::DragFloat("dashSpeed", &dashSpeed_, 0.01f);
		ImGui::DragFloat("dashDuration", &dashDuration_, 0.01f);
		ImGui::DragFloat("dashCooldown", &dashCooldown_, 0.01f);
	}

	ImGui::End();
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

void Base::UpdateDashCooldown(float deltaTime) {
	// ダッシュタイマーの減算
	if (dashCooldownTimer_ > 0.0f) {
		dashCooldownTimer_ -= deltaTime;
	}
}