#include "KillCounter.h"
#include <imgui/imgui.h>
#include <numbers>

using namespace SHEngine;

void KillCounter::Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager, SHEngine::TextureManager* textureManager) {
	int modelHandle = modelManager->LoadModel("Assets/.EngineResource/Model/plane");
	auto modelData = modelManager->GetNodeModelData(modelHandle);
	DrawData data = drawDataManager->GetDrawData(modelManager->GetNodeModelData(1).drawDataIndex);

	enemySprite_ = std::make_unique<RenderObject>("EnemySprite");
	enemySprite_->Initialize();
	enemySprite_->SetDrawData(data);
	enemySprite_->psoConfig_.vs = "Simple.VS.hlsl";
	enemySprite_->psoConfig_.ps = "TexColor.PS.hlsl";
	enemySprite_->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER, "WVP");
	enemySprite_->CreateCBV(sizeof(Vector4), ShaderType::PIXEL_SHADER, "Color");
	enemySprite_->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "TextureIndex");
	enemySprite_->SetUseTexture(true);

	counterText_ = std::make_unique<Text>();
	counterText_->Initialize(data, "YDWbananaslipplus.otf", 64, "x0");

	textTransform_.position = {1170.0f, -650.0f, 0.0f};
	textTransform_.rotate = {0.0f, 0.0f, 0.0f};
	textTransform_.scale = {1.0f, 1.0f, 1.0f};

	spriteTransform_.position = {1140.0f, -640.0f, 0.0f};
	spriteTransform_.rotate = {0.0f, 0.0f, std::numbers::pi_v<float> / 2.0f};
	spriteTransform_.scale = {34.0f, 34.0f, 1.0f};

	textureIndex_ = textureManager->LoadTexture("Enemy.png");
	baseScale_ = 1.0f;
}

void KillCounter::Update(float deltaTime, Matrix4x4 vpMatrix, int killCount) {
	Matrix4x4 world = Matrix::MakeAffineMatrix(spriteTransform_.scale, spriteTransform_.rotate, spriteTransform_.position);
	Matrix4x4 wvp = world * vpMatrix;
	Vector4 color = {1.0f, 1.0f, 1.0f, 1.0f};

	if (killCount > prevKillCount_) {
		scaleAnim_.anim.Start(baseScale_ * 1.6f, baseScale_, 0.25f, EaseType::EaseOutBack);
	}
	prevKillCount_ = killCount;

	bool playing = scaleAnim_.anim.Update(deltaTime, scaleAnim_.temp);

	if (playing) {
		textTransform_.scale.x = scaleAnim_.temp;
	}

	enemySprite_->CopyBufferData(0, &wvp, sizeof(Matrix4x4));
	enemySprite_->CopyBufferData(1, &color, sizeof(Vector4));
	enemySprite_->CopyBufferData(2, &textureIndex_, sizeof(int));

	counterText_->SetText(L"x" + std::to_wstring(killCount));
	counterText_->SetTransform(textTransform_);
	counterText_->Update(vpMatrix);

#ifdef USE_IMGUI
	ImGui::Begin("KillCounter");
	ImGui::DragFloat3("TextPosition", &textTransform_.position.x, 1.0f);
	ImGui::DragFloat3("SpritePosition", &spriteTransform_.position.x, 1.0f);
	ImGui::DragFloat3("TextScale", &textTransform_.scale.x, 0.01f);
	ImGui::End();
#endif
}

void KillCounter::Draw(CmdObj* cmdObj) {
	enemySprite_->Draw(cmdObj);
	counterText_->Draw(cmdObj);
}
