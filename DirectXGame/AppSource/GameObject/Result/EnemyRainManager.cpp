#include "EnemyRainManager.h"
#include "GameObject/Random/Random.h"
#include <numbers>

using namespace SHEngine;

void EnemyRainManager::Initilaize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager) {
	int modelHandle = modelManager->LoadModel("Assets/Model/Enemy/Normal");
	auto modelData = modelManager->GetNodeModelData(modelHandle);
	auto drawData = drawDataManager->GetDrawData(modelData.drawDataIndex);
	auto& material = modelData.materials[modelData.materialIndex.front()];
	textureIndex_ = material.textureIndex;

	render_ = std::make_unique<SHEngine::RenderObject>();
	render_->Initialize();
	render_->SetUseTexture(true);
	render_->SetDrawData(drawData);
	render_->psoConfig_.vs = "Simples.VS.hlsl";
	render_->psoConfig_.ps = "Game/LightTexColors.PS.hlsl";
	render_->instanceNum_ = kInstanceNum;
	render_->CreateSRV(sizeof(Matrix4x4), kInstanceNum, ShaderType::VERTEX_SHADER, "WVP");
	render_->CreateSRV(sizeof(Vector4), kInstanceNum, ShaderType::PIXEL_SHADER, "Color");
	render_->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "TextureIndex");
	render_->CreateCBV(sizeof(DirectionalLight), ShaderType::PIXEL_SHADER, "DirectionalLight");

	widthRange_ = {-15.0f, 15.0f};
	heightRange_ = {10.0f, 40.0f};

	for (int i = 0; i < kInstanceNum; ++i) {
		transforms_[i].scale = {1.0f, 1.0f, 1.0f};
		transforms_[i].rotate = {0.0f, 0.0f, RandomUtils::RangeFloat(0.0f, std::numbers::pi_v<float>)};
		transforms_[i].position = {RandomUtils::RangeFloat(widthRange_.x, widthRange_.y), RandomUtils::RangeFloat(heightRange_.x, heightRange_.y), RandomUtils::RangeFloat(10.0f, 20.0f)};
	}
}

void EnemyRainManager::Update(Matrix4x4 vpMatrix, float deltaTime, DirectionalLight dirLight) {
	Matrix4x4 wvpMatrices[kInstanceNum];
	for (int i = 0; i < kInstanceNum; ++i) {
		transforms_[i].position.y -= deltaTime * fallSpeed_;
		transforms_[i].rotate.z += deltaTime;
		wvpMatrices[i] = Matrix::MakeAffineMatrix(transforms_[i].scale, transforms_[i].rotate, transforms_[i].position);
		wvpMatrices[i] *= vpMatrix;
		colors_[i] = {1.0f, 0.0f, 0.0f, 1.0f};

		if (transforms_[i].position.y <= posYLimit_) {
			transforms_[i].rotate = {0.0f, 0.0f, RandomUtils::RangeFloat(0.0f, std::numbers::pi_v<float>)};
			transforms_[i].position = {RandomUtils::RangeFloat(widthRange_.x, widthRange_.y), RandomUtils::RangeFloat(heightRange_.x, heightRange_.y), RandomUtils::RangeFloat(10.0f, 20.0f)};
		}
	}

	render_->CopyBufferData(0, wvpMatrices, sizeof(Matrix4x4) * kInstanceNum);
	render_->CopyBufferData(1, colors_, sizeof(Vector4) * kInstanceNum);
	render_->CopyBufferData(2, &textureIndex_, sizeof(int));
	render_->CopyBufferData(3, &dirLight, sizeof(DirectionalLight));

#ifdef USE_IMGUI
	ImGui::Begin("EnemyRainManager");
	ImGui::DragFloat2("WidthRange", &widthRange_.x, 0.01f);
	ImGui::DragFloat2("HeightRange", &heightRange_.x, 0.01f);
	ImGui::DragFloat("PosYLimit", &posYLimit_, 0.01f);
	ImGui::End();
#endif
}

void EnemyRainManager::Draw(CmdObj* cmdObj) { render_->Draw(cmdObj); }
