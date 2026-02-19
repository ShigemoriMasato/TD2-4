#include "BackGround.h"
#include <imgui/imgui.h>

BackGround::~BackGround() {
	Save();
}

void BackGround::Initialize(SHEngine::DrawData& drawData, Camera* camera) {
	camera_ = camera;
	renderObject_ = std::make_unique<SHEngine::RenderObject>("BackGround");
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
	for (int i = 0; i < static_cast<int>(waves_.size()); ++i) {
		waves_[i].lifeTime += deltaTime;
		if (waves_[i].lifeTime >= waves_[i].lifeSpan) {
			waves_[i].strength = 0.0f;
		}
	}

	vsData_.parentMatrix = Matrix::MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.position);
	vsData_.vpMatrix = camera_->GetVPMatrix();
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

void BackGround::Draw(CmdObj* cmdObj) {
	renderObject_->CopyBufferData(0, &vsData_, sizeof(vsData_));
	if (!waves_.empty()) {
		renderObject_->CopyBufferData(1, waves_.data(), sizeof(Wave) * 8);
	}
	renderObject_->Draw(cmdObj);
}


void BackGround::Save() {
	binaryManager_->RegisterOutput(&vsData_.x);
	binaryManager_->RegisterOutput(&vsData_.y);
	binaryManager_->RegisterOutput(&vsData_.innerColor);
	binaryManager_->RegisterOutput(&vsData_.outerColor);
	
	binaryManager_->RegisterOutput(&transform_.scale);
	binaryManager_->RegisterOutput(&transform_.rotate);
	binaryManager_->RegisterOutput(&transform_.position);

	binaryManager_->RegisterOutput(&editWaveData_.strength);
	binaryManager_->RegisterOutput(&editWaveData_.speed);
	binaryManager_->RegisterOutput(&editWaveData_.tickness);
	binaryManager_->RegisterOutput(&editWaveData_.lifeSpan);
	binaryManager_->RegisterOutput(&editWaveData_.generatePosition);
	binaryManager_->RegisterOutput(&editWaveData_.color);
	binaryManager_->RegisterOutput(&editWaveData_.direction);

	binaryManager_->Write(saveFile_);
}

void BackGround::Load() {
	auto data = binaryManager_->Read(saveFile_);

	if (data.empty()) {
		return;
	}

	uint32_t index = 0;

	vsData_.x = binaryManager_->Reverse<int>(data);
	vsData_.y = binaryManager_->Reverse<int>(data);
	vsData_.innerColor = binaryManager_->Reverse<Vector4>(data);
	vsData_.outerColor = binaryManager_->Reverse<Vector4>(data);

	transform_.scale = binaryManager_->Reverse<Vector3>(data);
	transform_.rotate = binaryManager_->Reverse<Vector3>(data);
	transform_.position = binaryManager_->Reverse<Vector3>(data);

	editWaveData_.strength = binaryManager_->Reverse<float>(data);
	editWaveData_.speed = binaryManager_->Reverse<float>(data);
	editWaveData_.tickness = binaryManager_->Reverse<float>(data);
	editWaveData_.lifeSpan = binaryManager_->Reverse<float>(data);
	editWaveData_.generatePosition = binaryManager_->Reverse<Vector2>(data);
	editWaveData_.color = binaryManager_->Reverse<Vector4>(data);
	editWaveData_.direction = binaryManager_->Reverse<Vector3>(data);
}
