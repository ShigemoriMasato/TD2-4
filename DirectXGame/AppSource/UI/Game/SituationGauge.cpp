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

	transform_.scale = {15.0f, 500.0f, 0.0f};
	transform_.position = {100.0f, -430.0f, 0.0f};

	situationTelop_ = std::make_unique<SituationTelop>();
	situationTelop_->Initialize(modelManager, drawDataManager, 0);
}

void SituationGauge::Update(Matrix4x4 vpMatrix, float deltaTime, float enemySpawnCount, float weaponCount, std::unordered_map<Key, bool> key) {
	// 各自の最大値からターゲットとなる割合を算出
	float targetPlayerRatio = 0.0f;
	if (maxWeaponCount_ > 0.0f) {
		targetPlayerRatio = std::clamp(weaponCount / maxWeaponCount_, 0.0f, 1.0f);
	}

	float targetEnemyRatio = 0.0f;
	if (maxEnemyCount_ > 0.0f) {
		targetEnemyRatio = std::clamp(enemySpawnCount / maxEnemyCount_, 0.0f, 1.0f);
	}

	// ゲージの補間
	currentPlayerRatio_ += (targetPlayerRatio - currentPlayerRatio_) * lerpSpeed_ * deltaTime;
	currentEnemyRatio_ += (targetEnemyRatio - currentEnemyRatio_) * lerpSpeed_ * deltaTime;

	// Xスケールを各バーの幅、Yスケールを最大高さとして扱う
	float barWidth = transform_.scale.x;
	float maxHeight = transform_.scale.y;

	// 高さを独立した割合に応じて計算
	float playerHeight = maxHeight * currentPlayerRatio_;
	float enemyHeight = maxHeight * currentEnemyRatio_;

	// 横に並べるためのX座標計算
	float playerPosX = transform_.position.x - (barWidth * 0.5f) - (barSpacing_ * 0.5f);
	float enemyPosX = transform_.position.x + (barWidth * 0.5f) + (barSpacing_ * 0.5f);

	// 下端揃えにするためのY座標計算
	float basePosY = transform_.position.y - (maxHeight * 0.5f);
	float playerPosY = basePosY + (playerHeight * 0.5f);
	float enemyPosY = basePosY + (enemyHeight * 0.5f);

	Matrix4x4 wvpMatrices[2];
	Vector4 colors[2];

	// プレイヤー用ゲージ
	Vector3 playerScale = {barWidth, playerHeight, transform_.scale.z};
	Vector3 playerPos = {playerPosX, playerPosY, transform_.position.z};
	Matrix4x4 playerWorld = Matrix::MakeAffineMatrix(playerScale, transform_.rotate, playerPos);
	wvpMatrices[0] = playerWorld * vpMatrix;
	colors[0] = playerColor_;

	// 敵用ゲージ
	Vector3 enemyScale = {barWidth, enemyHeight, transform_.scale.z};
	Vector3 enemyPos = {enemyPosX, enemyPosY, transform_.position.z};
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
	ImGui::DragFloat("Max Weapon Count", &maxWeaponCount_, 1.0f, 1.0f, 1000.0f);
	ImGui::DragFloat("Max Enemy Count", &maxEnemyCount_, 1.0f, 1.0f, 1000.0f);
	ImGui::DragFloat("Lerp Speed", &lerpSpeed_, 0.1f, 0.1f, 20.0f);
	ImGui::Text("Player Ratio: %.3f", currentPlayerRatio_);
	ImGui::Text("Enemy Ratio: %.3f", currentEnemyRatio_);
	ImGui::End();
#endif

	render_->CopyBufferData(0, wvpMatrices, sizeof(Matrix4x4) * kGaugeCount);
	render_->CopyBufferData(1, colors, sizeof(Vector4) * kGaugeCount);
	render_->CopyBufferData(2, &textureIndex, sizeof(int));

	currentAdvantage_ = GetAdvantage(playerScale.y, enemyScale.y); // プレイヤーが有利かどうかの判定

	// 状況が変化したと判断してテロップを表示
	if (currentAdvantage_ != wasAdvantage_) {
		// プレイヤーのゲージが敵のゲージの2倍以上の場合は不利、敵のゲージがプレイヤーのゲージの2倍以上の場合は有利と表示
		if (currentAdvantage_ == Advantage::Player) {
			situationTelop_->StartAnimation(L"有利");
		} else if (currentAdvantage_ == Advantage::Enemy) {
			situationTelop_->StartAnimation(L"不利");
		}
	}

	// 有利不利を記録
	wasAdvantage_ = currentAdvantage_;

	situationTelop_->Update(vpMatrix, key, deltaTime);
}

void SituationGauge::Draw(CmdObj* cmdObj) {
	render_->Draw(cmdObj);
	situationTelop_->Draw(cmdObj);
}

SituationGauge::Advantage SituationGauge::GetAdvantage(float player, float enemy) const {
	if (player > enemy * 2.0f) {
		return Advantage::Player;
	}
	if (enemy > player * 2.0f) {
		return Advantage::Enemy;
	}
	return Advantage::Even;
}
