#include "Player.h"
#include <Utility/Matrix.h>
#include <Utility/MatrixFactory.h>
#include <imgui/imgui.h>

using namespace SHEngine;

void Player::Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager, Input* input) {
	render_ = std::make_unique<RenderObject>();
	render_->Initialize();
	render_->psoConfig_.vs = "Simple.VS.hlsl"; // WPだけ送るVS
	render_->psoConfig_.ps = "White.PS.hlsl";  // とりあえず白で返すPS

	render_->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER);

	int modelHandle = modelManager->LoadModel("Assets/.EngineResource/Model/Cube");
	// int modelHandle;
	// modelHandle = 0; //Cube
	// modelHandle = 1; //plane
	// 詳しくはModelManagerのInitializeへ

	// 描画するデータの読み込み
	auto modelData = modelManager->GetNodeModelData(modelHandle);
	auto drawData = drawDataManager->GetDrawData(modelData.drawDataIndex);
	render_->SetDrawData(drawData);

	// 単位行列の代入
	wvp_ = Matrix4x4::Identity();

	// 入力
	input_ = input;

	// 状態の初期化
	currentState_ = std::make_unique<PlayerStateNormal>(); // 通常

	// FPSObserver
	fpsObserver_ = std::make_unique<FPSObserver>();
}

void Player::Update(Matrix4x4 vpMatrix) {
	// 現在の状態の更新処理
	if (currentState_) {
		currentState_->Update(this, fpsObserver_->GetDeltatime());
	}

	// 座標を行列に変換
	wvp_ = Matrix::MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.position);
	wvp_ *= vpMatrix;

	// wvp行列を描画に適用
	render_->CopyBufferData(0, &wvp_, sizeof(Matrix4x4));
}

void Player::Draw(CmdObj* cmdObj) {
	// 描画
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

void Player::ChangeState(std::unique_ptr<IPlayerState> newState) {
	if (currentState_) {
		currentState_->Exit(this); // 古い状態の終了処理
	}

	// 状態の切り替え
	currentState_ = std::move(newState);

	if (currentState_) {
		currentState_->Enter(this); // 新しい状態の開始処理
	}
}

void Player::UpdateDashCooldown() {
	// ダッシュタイマーの減算
	if (dashCooldownTimer_ > 0.0f) {
		dashCooldownTimer_ -= fpsObserver_->GetDeltatime();
	}
}