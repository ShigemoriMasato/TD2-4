#include "ResultSword.h"
#include <numbers>

#ifdef USE_IMGUI
#include <imgui/imgui.h>
#endif

using namespace SHEngine;

void ResultSword::Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager, SHEngine::TextureManager* textureManager) {
	int modelHandle = modelManager->LoadModel("Assets/Model/Item/Weapon/Sword");
	auto modelData = modelManager->GetNodeModelData(modelHandle);
	DrawData data = drawDataManager->GetDrawData(modelData.drawDataIndex);

	renderSword1_ = std::make_unique<RenderObject>("ResultSword1");
	renderSword1_->Initialize();
	renderSword1_->SetDrawData(data);
	renderSword1_->psoConfig_.vs = "Simple.VS.hlsl";
	renderSword1_->psoConfig_.ps = "TexColor.PS.hlsl";
	renderSword1_->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER, "WVP");
	renderSword1_->CreateCBV(sizeof(Vector4), ShaderType::PIXEL_SHADER, "Color");
	renderSword1_->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "TextureIndex");
	renderSword1_->SetUseTexture(true);

	renderSword2_ = std::make_unique<RenderObject>("ResultSword2");
	renderSword2_->Initialize();
	renderSword2_->SetDrawData(data);
	renderSword2_->psoConfig_.vs = "Simple.VS.hlsl";
	renderSword2_->psoConfig_.ps = "TexColor.PS.hlsl";
	renderSword2_->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER, "WVP");
	renderSword2_->CreateCBV(sizeof(Vector4), ShaderType::PIXEL_SHADER, "Color");
	renderSword2_->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "TextureIndex");
	renderSword2_->SetUseTexture(true);

	auto& material = modelData.materials[modelData.materialIndex.front()];
	textureIndex_ = material.textureIndex;
	transformSword1_.scale = {1.0f, 1.0f, 1.0f};
	transformSword1_.rotate = {std::numbers::pi_v<float> / 2.0f, 0.0f, std::numbers::pi_v<float> * 4.0f};
	transformSword1_.position = {-13.0f, 0.0f, 0.0f};
	transformSword2_.scale = {1.0f, 1.0f, 1.0f};
	transformSword2_.rotate = {std::numbers::pi_v<float> / 2.0f, 0.0f, -std::numbers::pi_v<float> * 4.0f};
	transformSword2_.position = {13.0f, 0.0f, 0.0f};
}

void ResultSword::StartAnimation() {
	float duration = 1.0f;
	EaseType easeType = EaseType::EaseInBack;

	// 剣1
	Vector3 s1StartPos = {-13.0f, 0.0f, 0.0f};
	Vector3 s1EndPos = {0.0f, 0.0f, 0.0f};
	Vector3 s1StartRot = {std::numbers::pi_v<float> / 2.0f, 0.0f, std::numbers::pi_v<float> * 4.0f};
	Vector3 s1EndRot = {std::numbers::pi_v<float> / 2.0f, 0.0f, std::numbers::pi_v<float> * 3.0f / 4.0f};

	sword1PosAnim_.Start(s1StartPos, s1EndPos, duration, easeType);
	sword1RotAnim_.Start(s1StartRot, s1EndRot, duration, easeType);

	// 剣2
	Vector3 s2StartPos = {13.0f, 0.0f, 0.0f};
	Vector3 s2EndPos = {0.0f, 0.0f, 0.0f};
	Vector3 s2StartRot = {std::numbers::pi_v<float> / 2.0f, 0.0f, -std::numbers::pi_v<float> * 4.0f};
	Vector3 s2EndRot = {std::numbers::pi_v<float> / 2.0f, 0.0f, -std::numbers::pi_v<float> * 3.0f / 4.0f};

	sword2PosAnim_.Start(s2StartPos, s2EndPos, duration, easeType);
	sword2RotAnim_.Start(s2StartRot, s2EndRot, duration, easeType);

	transformSword1_.position = s1StartPos;
	transformSword1_.rotate = s1StartRot;
	transformSword2_.position = s2StartPos;
	transformSword2_.rotate = s2StartRot;

	isAnimationFinished_ = false;
}

void ResultSword::Update(Matrix4x4 vpMatrix, float deltaTime) {
	// アニメーションの更新
	if (!isAnimationFinished_) {
		bool isS1PosActive = sword1PosAnim_.Update(deltaTime, transformSword1_.position);
		bool isS1RotActive = sword1RotAnim_.Update(deltaTime, transformSword1_.rotate);
		bool isS2PosActive = sword2PosAnim_.Update(deltaTime, transformSword2_.position);
		bool isS2RotActive = sword2RotAnim_.Update(deltaTime, transformSword2_.rotate);

		// 全てのアニメーションが完了したら終了フラグを立てる
		if (!isS1PosActive && !isS1RotActive && !isS2PosActive && !isS2RotActive) {
			isAnimationFinished_ = true;
		}
	}

	Vector4 color = {1.0f, 1.0f, 1.0f, 1.0f};

	Matrix4x4 s1World = Matrix::MakeAffineMatrix(transformSword1_.scale, transformSword1_.rotate, transformSword1_.position);
	Matrix4x4 s1WVP = s1World * vpMatrix;

	renderSword1_->CopyBufferData(0, &s1WVP, sizeof(Matrix4x4));
	renderSword1_->CopyBufferData(1, &color, sizeof(Vector4));
	renderSword1_->CopyBufferData(2, &textureIndex_, sizeof(int));

	Matrix4x4 s2World = Matrix::MakeAffineMatrix(transformSword2_.scale, transformSword2_.rotate, transformSword2_.position);
	Matrix4x4 s2WVP = s2World * vpMatrix;

	renderSword2_->CopyBufferData(0, &s2WVP, sizeof(Matrix4x4));
	renderSword2_->CopyBufferData(1, &color, sizeof(Vector4));
	renderSword2_->CopyBufferData(2, &textureIndex_, sizeof(int));

#ifdef USE_IMGUI
	ImGui::Begin("Sword Setting");

	if (ImGui::CollapsingHeader("Sword 1")) {
		ImGui::PushID("Sword1");

		ImGui::DragFloat3("Position", &transformSword1_.position.x, 0.1f);
		ImGui::DragFloat3("Rotate", &transformSword1_.rotate.x, 0.1f);
		ImGui::DragFloat3("Scale", &transformSword1_.scale.x, 0.1f);

		ImGui::PopID();
	}

	if (ImGui::CollapsingHeader("Sword 2")) {
		ImGui::PushID("Sword2");

		ImGui::DragFloat3("Position", &transformSword2_.position.x, 0.1f);
		ImGui::DragFloat3("Rotate", &transformSword2_.rotate.x, 0.1f);
		ImGui::DragFloat3("Scale", &transformSword2_.scale.x, 0.1f);

		ImGui::PopID();
	}

	ImGui::End();
#endif
}

void ResultSword::Draw(CmdObj* cmdObj) {
	renderSword1_->Draw(cmdObj);
	renderSword2_->Draw(cmdObj);
}

bool ResultSword::IsAnimationFinished() const { return isAnimationFinished_; }