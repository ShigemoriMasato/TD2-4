#include "SituationGauge.h"
#ifdef USE_IMGUI
#include <imgui/imgui.h>
#endif // USE_IMGUI

using namespace SHEngine;

void SituationGauge::Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager) {
	int modelHandle = modelManager->LoadModel("Assets/.EngineResource/Model/plane");
	auto modelData = modelManager->GetNodeModelData(modelHandle);
	DrawData data = drawDataManager->GetDrawData(modelData.drawDataIndex);

	render_ = std::make_unique<RenderObject>("GaugeUI");
	render_->Initialize();
	render_->SetDrawData(data);
	render_->psoConfig_.vs = "Simples.VS.hlsl";
	render_->psoConfig_.ps = "TexColors.PS.hlsl";
	render_->CreateSRV(sizeof(Matrix4x4), kGaugeCount, ShaderType::VERTEX_SHADER, "WVP");
	render_->CreateSRV(sizeof(Vector4), kGaugeCount, ShaderType::PIXEL_SHADER, "Color");
	render_->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "TextureIndex");
	render_->SetUseTexture(true);
	render_->instanceNum_ = kGaugeCount;

	transform_.scale = {500.0f, 50.0f, 0.0f};
	transform_.position = {620.0f, -665.0f, 0.0f};
}

void SituationGauge::Update(Matrix4x4 vpMatrix, float deltaTime, float enemySpawnCount, float weaponCount) {
	// 戦況の計算
	float playerPower = weaponCount * weaponPowerWeight_;
	float enemyPower = enemySpawnCount * enemyPowerWeight_;
	float totalPower = playerPower + enemyPower;

	float targetIntensity = 0.5f;
	if (totalPower > 0.0f) {
		targetIntensity = playerPower / totalPower;
	}
	targetIntensity = std::clamp(targetIntensity, 0.0f, 1.0f);

	// ゲージの補間
	currentIntensity_ += (targetIntensity - currentIntensity_) * lerpSpeed_ * deltaTime;

	float playerRatio = currentIntensity_;
	float enemyRatio = 1.0f - currentIntensity_;

	// 全体の幅からそれぞれの幅を計算
	float totalWidth = transform_.scale.x;
	float playerWidth = totalWidth * playerRatio;
	float enemyWidth = totalWidth * enemyRatio;

	// 左から右へゲージが並ぶように位置を計算
	float leftEdgeX = transform_.position.x - (totalWidth * 0.5f);
	float playerPosX = leftEdgeX + (playerWidth * 0.5f);
	float enemyPosX = leftEdgeX + playerWidth + (enemyWidth * 0.5f);

	Matrix4x4 wvpMatrices[2];
	Vector4 colors[2];

	// プレイヤー用ゲージ
	Vector3 playerScale = {playerWidth, transform_.scale.y, transform_.scale.z};
	Vector3 playerPos = {playerPosX, transform_.position.y, transform_.position.z};
	Matrix4x4 playerWorld = Matrix::MakeAffineMatrix(playerScale, transform_.rotate, playerPos);
	wvpMatrices[0] = playerWorld * vpMatrix;
	colors[0] = playerColor_;

	// 敵用ゲージ
	Vector3 enemyScale = {enemyWidth, transform_.scale.y, transform_.scale.z};
	Vector3 enemyPos = {enemyPosX, transform_.position.y, transform_.position.z};
	Matrix4x4 enemyWorld = Matrix::MakeAffineMatrix(enemyScale, transform_.rotate, enemyPos);
	wvpMatrices[1] = enemyWorld * vpMatrix;
	colors[1] = enemyColor_;

#ifdef USE_IMGUI
	ImGui::Begin("GaugeUI");
	ImGui::DragFloat3("Position", &transform_.position.x, 0.01f);
	ImGui::DragFloat2("Scale", &transform_.scale.x, 0.01f);
	ImGui::ColorEdit4("Player Color", &playerColor_.x);
	ImGui::ColorEdit4("Enemy Color", &enemyColor_.x);
	ImGui::Separator();
	ImGui::Text("Battle Situation");
	ImGui::DragFloat("Weapon Power Weight", &weaponPowerWeight_, 0.1f, 0.0f, 100.0f);
	ImGui::DragFloat("Enemy Power Weight", &enemyPowerWeight_, 0.1f, 0.0f, 100.0f);
	ImGui::DragFloat("Lerp Speed", &lerpSpeed_, 0.1f, 0.1f, 20.0f);
	ImGui::Text("Target Intensity: %.3f", targetIntensity);
	ImGui::Text("Current Intensity: %.3f", currentIntensity_);
	ImGui::End();
#endif

	render_->CopyBufferData(0, wvpMatrices, sizeof(Matrix4x4) * kGaugeCount);
	render_->CopyBufferData(1, colors, sizeof(Vector4) * kGaugeCount);
	render_->CopyBufferData(2, &textureIndex, sizeof(int));
}

void SituationGauge::Draw(CmdObj* cmdObj) { render_->Draw(cmdObj); }