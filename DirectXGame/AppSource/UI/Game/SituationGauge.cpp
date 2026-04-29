#include "SituationGauge.h"
#ifdef USE_IMGUI
#include <imgui/imgui.h>
#endif // USE_IMGUI

using namespace SHEngine;

namespace {
struct GlobalData {
	int textureIndex;
	float time;
	Vector2 globalPadding;
};

struct InstanceData {
	Vector4 color;
	float intensity;
	float width;
	float height;
	float padding;
};
} // namespace

void SituationGauge::Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager, SHEngine::TextureManager* textureManager) {
	int modelHandle = modelManager->LoadModel("Assets/.EngineResource/Model/plane");
	auto modelData = modelManager->GetNodeModelData(modelHandle);
	DrawData data = drawDataManager->GetDrawData(modelData.drawDataIndex);

	render_ = std::make_unique<RenderObject>("GaugeUI");
	render_->Initialize();
	render_->SetDrawData(data);
	render_->psoConfig_.vs = "Simples.VS.hlsl";
	render_->psoConfig_.ps = "Game/DistortionGauge.PS.hlsl";
	render_->CreateSRV(sizeof(Matrix4x4), gaugeCount_, ShaderType::VERTEX_SHADER, "WVP");
	render_->CreateSRV(sizeof(InstanceData), gaugeCount_, ShaderType::PIXEL_SHADER, "InstanceData");
	render_->CreateCBV(sizeof(GlobalData), ShaderType::PIXEL_SHADER, "GlobalData");
	render_->SetUseTexture(true);
	render_->instanceNum_ = gaugeCount_;

	transform_.scale = {50.0f, 350.0f, 0.0f};
	transform_.position = {1100.0f, -450.0f, 0.0f};

	transformWeaponIcon_.scale = {34.0f, 34.0f, 0.0f};
	transformWeaponIcon_.rotate = {0.0f, 0.0f, std::numbers::pi_v<float> / 2.0f};
	transformWeaponIcon_.position = {1055.0f, -650.0f, 0.0f};

	transformEnemyIcon_.scale = {34.0f, 34.0f, 0.0f};
	transformEnemyIcon_.rotate = {0.0f, 0.0f, std::numbers::pi_v<float> / 2.0f};
	transformEnemyIcon_.position = {1150.0f, -650.0f, 0.0f};

	situationTelop_ = std::make_unique<SituationTelop>();
	situationTelop_->Initialize(modelManager, drawDataManager, 0);

	renderWeaponIcon_ = std::make_unique<RenderObject>("WeaponIcon");
	renderWeaponIcon_->Initialize();
	renderWeaponIcon_->SetDrawData(data);
	renderWeaponIcon_->psoConfig_.vs = "Simple.VS.hlsl";
	renderWeaponIcon_->psoConfig_.ps = "TexColor.PS.hlsl";
	renderWeaponIcon_->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER, "WVP");
	renderWeaponIcon_->CreateCBV(sizeof(Vector4), ShaderType::PIXEL_SHADER, "Color");
	renderWeaponIcon_->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "TextureIndex");
	renderWeaponIcon_->SetUseTexture(true);

	renderEnemyIcon_ = std::make_unique<RenderObject>("EnemyIcon");
	renderEnemyIcon_->Initialize();
	renderEnemyIcon_->SetDrawData(data);
	renderEnemyIcon_->psoConfig_.vs = "Simple.VS.hlsl";
	renderEnemyIcon_->psoConfig_.ps = "TexColor.PS.hlsl";
	renderEnemyIcon_->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER, "WVP");
	renderEnemyIcon_->CreateCBV(sizeof(Vector4), ShaderType::PIXEL_SHADER, "Color");
	renderEnemyIcon_->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "TextureIndex");
	renderEnemyIcon_->SetUseTexture(true);

	wIconTextureIndex_ = textureManager->LoadTexture("Sword.png");
	eIconTextureIndex_ = textureManager->LoadTexture("Enemy.png");
}

void SituationGauge::Update(Matrix4x4 vpMatrix, float deltaTime, float enemySpawnCount, float weaponCount, std::unordered_map<Key, bool> key) {
	time_ += deltaTime;

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
	float playerHeight = std::max(maxHeight * currentPlayerRatio_, minGaugeHeight_);
	float enemyHeight = std::max(maxHeight * currentEnemyRatio_, minGaugeHeight_);

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
	ImGui::DragFloat3("enemyPos", &transformEnemyIcon_.position.x, 1.0f);
	ImGui::DragFloat3("weaponPos", &transformWeaponIcon_.position.x, 1.0f);
	ImGui::Separator();
	ImGui::Text("Battle Situation");
	ImGui::DragFloat("Max Weapon Count", &maxWeaponCount_, 1.0f, 1.0f, 1000.0f);
	ImGui::DragFloat("Max Enemy Count", &maxEnemyCount_, 1.0f, 1.0f, 1000.0f);
	ImGui::DragFloat("Lerp Speed", &lerpSpeed_, 0.1f, 0.1f, 20.0f);
	ImGui::Text("Player Ratio: %.3f", currentPlayerRatio_);
	ImGui::Text("Enemy Ratio: %.3f", currentEnemyRatio_);
	ImGui::End();
#endif

	InstanceData instanceDataArray[2];

	// プレイヤー用ゲージデータ
	instanceDataArray[0].color = playerColor_;
	instanceDataArray[0].intensity = currentPlayerRatio_;
	instanceDataArray[0].width = barWidth;      // 追加
	instanceDataArray[0].height = playerHeight; // 追加

	// 敵用ゲージデータ
	instanceDataArray[1].color = enemyColor_;
	instanceDataArray[1].intensity = currentEnemyRatio_;
	instanceDataArray[1].width = barWidth;     // 追加
	instanceDataArray[1].height = enemyHeight; // 追加

	GlobalData globalData;
	globalData.textureIndex = textureIndex_;
	globalData.time = time_;

	render_->CopyBufferData(0, wvpMatrices, sizeof(Matrix4x4) * gaugeCount_);
	render_->CopyBufferData(1, instanceDataArray, sizeof(InstanceData) * gaugeCount_);
	render_->CopyBufferData(2, &globalData, sizeof(GlobalData));

	Matrix4x4 wIconWorld = Matrix::MakeAffineMatrix(transformWeaponIcon_.scale, transformWeaponIcon_.rotate, transformWeaponIcon_.position);
	Matrix4x4 wIconWVP = wIconWorld * vpMatrix;
	Vector4 color = {1.0f, 1.0f, 1.0f, 1.0f};

	renderWeaponIcon_->CopyBufferData(0, &wIconWVP, sizeof(Matrix4x4));
	renderWeaponIcon_->CopyBufferData(1, &color, sizeof(Vector4));
	renderWeaponIcon_->CopyBufferData(2, &wIconTextureIndex_, sizeof(int));

	Matrix4x4 eIconWorld = Matrix::MakeAffineMatrix(transformEnemyIcon_.scale, transformEnemyIcon_.rotate, transformEnemyIcon_.position);
	Matrix4x4 eIconWVP = eIconWorld * vpMatrix;

	renderEnemyIcon_->CopyBufferData(0, &eIconWVP, sizeof(Matrix4x4));
	renderEnemyIcon_->CopyBufferData(1, &color, sizeof(Vector4));
	renderEnemyIcon_->CopyBufferData(2, &eIconTextureIndex_, sizeof(int));

	currentAdvantage_ = GetAdvantage(currentPlayerRatio_, currentEnemyRatio_); // プレイヤーが有利かどうかの判定

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
	renderWeaponIcon_->Draw(cmdObj);
	renderEnemyIcon_->Draw(cmdObj);
	situationTelop_->Draw(cmdObj);
}

SituationGauge::Advantage SituationGauge::GetAdvantage(float playerRatio, float enemyRatio) const {
	if (playerRatio > enemyRatio * 2.0f) {
		return Advantage::Player;
	}
	if (enemyRatio > playerRatio * 2.0f) {
		return Advantage::Enemy;
	}
	return Advantage::Even;
}
