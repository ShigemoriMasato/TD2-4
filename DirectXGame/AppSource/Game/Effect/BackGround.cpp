#include "BackGround.h"
#include <imgui/imgui.h>

BackGround::~BackGround() {
	Save();
}

void BackGround::Initialize(DrawData& drawData, Camera* camera) {
	camera_ = camera;
	renderObject_ = std::make_unique<RenderObject>("BackGround");
	renderObject_->Initialize();
	renderObject_->SetDrawData(drawData);
	renderObject_->CreateCBV(sizeof(VSData), ShaderType::VERTEX_SHADER, "BackGround.VSData");
	renderObject_->CreateSRV(sizeof(Wave), 8, ShaderType::VERTEX_SHADER, "BackGround.WaveSRV");
	renderObject_->psoConfig_.vs = "Game/BackGround.VS.hlsl";
	renderObject_->psoConfig_.ps = "Game/Block.PS.hlsl";
	vsData_.x = 20;
	vsData_.y = 10;
	vsData_.innerColor = { 0.1f, 0.1f, 0.3f, 1.0f };
	vsData_.outerColor = { 0.4f, 0.4f, 0.4f, 1.0f };
	vsData_.parentMatrix = Matrix::MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.position);

	waves_.resize(8);

	binaryManager_ = std::make_unique<BinaryManager>();

	Load();

	vsData_.parentMatrix = Matrix::MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.position);
	renderObject_->instanceNum_ = vsData_.x * vsData_.y;
	editWaveData_.direction = editWaveData_.direction.Normalize();
}

void BackGround::Update(float deltaTime) {
	for(int i = 0; i < static_cast<int>(waves_.size()); ++i) {
		waves_[i].lifeTime += deltaTime;
		if (waves_[i].lifeTime >= waves_[i].lifeSpan) {
			waves_[i].strength = 0.0f;
		}
	}

	vsData_.vpMatrix = camera_->GetVPMatrix();
	vsData_.parentMatrix = Matrix::MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.position);
	renderObject_->instanceNum_ = vsData_.x * vsData_.y;
}

void BackGround::AddWave(const Wave& waveData) {
	for(int i = 0; i < static_cast<int>(waves_.size()); ++i) {
		if (waves_[i].lifeTime >= waves_[i].lifeSpan) {
			waves_[i] = waveData;
			break;
		}
	}
}

void BackGround::DrawImGui() {
#ifdef USE_IMGUI

	ImGui::Begin("BackGround");

	ImGui::ColorEdit4("InnerColor", &vsData_.innerColor.x);
	ImGui::ColorEdit4("OuterColor", &vsData_.outerColor.x);
	ImGui::DragFloat3("Scale", &transform_.scale.x, 0.05f); 
	ImGui::DragFloat3("Rotate", &transform_.rotate.x, 0.01f);
	ImGui::DragFloat3("Position", &transform_.position.x, 0.1f);
	ImGui::DragInt2("BlockNum", &vsData_.x, 1.0f, 1, 10000);

	ImGui::Separator();

	if (ImGui::Button("Add")) {
		AddWave(editWaveData_);
	}
	ImGui::DragFloat("Strength", &editWaveData_.strength, 0.1f, 0.0f, 100.0f);
	ImGui::DragFloat("Speed", &editWaveData_.speed, 0.1f, 0.0f, 100.0f);
	ImGui::DragFloat("Tickness", &editWaveData_.tickness, 0.1f, 0.0f, 100.0f);
	ImGui::DragFloat("LifeSpan", &editWaveData_.lifeSpan, 0.1f, 0.0f, 100.0f);
	ImGui::DragFloat2("GeneratePosition", &editWaveData_.generatePosition.x, 0.1f, -50.0f, 50.0f);
	ImGui::ColorEdit4("Color", &editWaveData_.color.x);
	ImGui::DragFloat3("Direction", &editWaveData_.direction.x, 0.05f, -1.0f, 1.0f);
	
	ImGui::End();

#endif
}

void BackGround::Draw(Window* window) {
	renderObject_->CopyBufferData(0, &vsData_, sizeof(vsData_));
	if (!waves_.empty()) {
		renderObject_->CopyBufferData(1, waves_.data(), sizeof(Wave) * 8);
	}
	renderObject_->Draw(window);
}


void BackGround::Save() {
	binaryManager_->RegistOutput(vsData_.x);
	binaryManager_->RegistOutput(vsData_.y);
	binaryManager_->RegistOutput(vsData_.innerColor);
	binaryManager_->RegistOutput(vsData_.outerColor);
	
	binaryManager_->RegistOutput(transform_.scale);
	binaryManager_->RegistOutput(transform_.rotate);
	binaryManager_->RegistOutput(transform_.position);

	binaryManager_->RegistOutput(editWaveData_.strength);
	binaryManager_->RegistOutput(editWaveData_.speed);
	binaryManager_->RegistOutput(editWaveData_.tickness);
	binaryManager_->RegistOutput(editWaveData_.lifeSpan);
	binaryManager_->RegistOutput(editWaveData_.generatePosition);
	binaryManager_->RegistOutput(editWaveData_.color);
	binaryManager_->RegistOutput(editWaveData_.direction);

	binaryManager_->Write(saveFile_);
}

void BackGround::Load() {
	auto data = binaryManager_->Read(saveFile_);

	if (data.empty()) {
		return;
	}

	uint32_t index = 0;

	vsData_.x = BinaryManager::Reverse<int>(data[index++]);
	vsData_.y = BinaryManager::Reverse<int>(data[index++]);
	vsData_.innerColor = BinaryManager::Reverse<Vector4>(data[index++]);
	vsData_.outerColor = BinaryManager::Reverse<Vector4>(data[index++]);

	transform_.scale = BinaryManager::Reverse<Vector3>(data[index++]);
	transform_.rotate = BinaryManager::Reverse<Vector3>(data[index++]);
	transform_.position = BinaryManager::Reverse<Vector3>(data[index++]);

	editWaveData_.strength = BinaryManager::Reverse<float>(data[index++]);
	editWaveData_.speed = BinaryManager::Reverse<float>(data[index++]);
	editWaveData_.tickness = BinaryManager::Reverse<float>(data[index++]);
	editWaveData_.lifeSpan = BinaryManager::Reverse<float>(data[index++]);
	editWaveData_.generatePosition = BinaryManager::Reverse<Vector2>(data[index++]);
	editWaveData_.color = BinaryManager::Reverse<Vector4>(data[index++]);
	editWaveData_.direction = BinaryManager::Reverse<Vector3>(data[index++]);
}
