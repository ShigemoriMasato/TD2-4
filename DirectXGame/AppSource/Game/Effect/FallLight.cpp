#include "FallLight.h"
#include <imgui/imgui.h>

FallLight::~FallLight() {

}

void FallLight::Initialize(const DrawData& drawData, TextureManager* textureManager, Camera* camera) {
	renderObject_ = std::make_unique<RenderObject>("FallLight");
	renderObject_->Initialize();
	renderObject_->CreateSRV(sizeof(VSData), maxInstance_, ShaderType::VERTEX_SHADER, "FallLight::VSData");
	renderObject_->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "FallLight::TextureIndex");
	renderObject_->psoConfig_.vs = "Game/FallLight.VS.hlsl";
	renderObject_->psoConfig_.ps = "Game/FallLight.PS.hlsl";
	renderObject_->SetDrawData(drawData);
	renderObject_->SetUseTexture(true);
	renderObject_->instanceNum_ = maxInstance_;
	camera_ = camera;
	textureManager_ = textureManager;
	textureIndex_ = textureManager_->LoadTexture(textureFile_);
	renderObject_->CopyBufferData(1, &textureIndex_, sizeof(int));

	isActive_.resize(maxInstance_, false);
	instanceData_.resize(maxInstance_);
	std::random_device rd;
	rand_.seed(rd());

	binaryManager_ = std::make_unique<BinaryManager>();
	Load();
}

void FallLight::Update(float deltaTime) {
	timer_ += deltaTime;

	while (timer_ > castTime_) {
		timer_ -= castTime_;

		bool backFirst = false;
		uint32_t i;
		for (int j = 0; j < 2; ++j) {
			for (i = searchIndex_; i < maxInstance_; ++i) {
				if (!isActive_[i]) {
					break;
				}
			}
			if (i != maxInstance_) {
				break;
			}
		}
		if (i == maxInstance_) {
			break;
		}

		VSData& data = instanceData_[i];

		data.lifeSpan = lifeSpan_;
		data.Speed = speed_;
		std::uniform_real_distribution<float> dirDist(-1.0f, 1.0f);
		data.dir = { dirDist(rand_), dirDist(rand_), dirDist(rand_) };
		data.dir = data.dir.Normalize();
		std::uniform_real_distribution<float> positionDist(-50.0f, 50.0f);
		data.firstPosition = { positionDist(rand_), positionDist(rand_), positionDist(rand_) };
		std::uniform_real_distribution<float> colorDist(minColorParam_, 1.0f);
		data.color = { colorDist(rand_), colorDist(rand_), colorDist(rand_) };
		data.timer = 0.0f;

		isActive_[i] = true;
		activeNum_++;

		searchIndex_ = ++i;
	}

	for (uint32_t i = 0; i < maxInstance_; ++i) {
		if (!isActive_[i]) {
			continue;
		}
		VSData& data = instanceData_[i];
		data.timer += deltaTime;
		data.vp = camera_->GetVPMatrix();
		if (data.timer > data.lifeSpan) {
			isActive_[i] = false;
			activeNum_--;
		}
	}
}

void FallLight::Draw(Window* window) {
	renderObject_->CopyBufferData(0, instanceData_.data(), sizeof(VSData) * static_cast<size_t>(instanceData_.size()));
	renderObject_->Draw(window);
}

void FallLight::DrawImGui() {
#ifdef USE_IMGUI

	ImGui::Begin("FallLight");
	ImGui::Text("Active : %d", activeNum_);
	ImGui::InputText("TexturePath", texturePathBuffer_, 256);
	if (ImGui::Button("LoadTexture")) {
		textureFile_ = std::string(texturePathBuffer_);
		textureIndex_ = textureManager_->LoadTexture(textureFile_);
		renderObject_->CopyBufferData(1, &textureIndex_, sizeof(int));
	}
	ImGui::DragFloat("CastTime", &castTime_, 0.01f);
	ImGui::DragFloat("LifeSpan", &lifeSpan_, 0.01f);
	ImGui::DragFloat("MinColorParam", &minColorParam_, 0.01f, 0.0f, 1.0f);

	ImGui::End();

#endif
}

void FallLight::Save() {
	binaryManager_->RegistOutput(textureFile_);

	binaryManager_->RegistOutput(castTime_);
	binaryManager_->RegistOutput(lifeSpan_);
	binaryManager_->RegistOutput(minColorParam_);

	binaryManager_->Write(saveFile_);
}

void FallLight::Load() {
	auto data = binaryManager_->Read(saveFile_);

	if (data.empty()) {
		return;
	}

	uint32_t index = 0;

	textureFile_ = binaryManager_->Reverse<std::string>(data[index++]);

	castTime_ = binaryManager_->Reverse<float>(data[index++]);
	lifeSpan_ = binaryManager_->Reverse<float>(data[index++]);
	minColorParam_ = binaryManager_->Reverse<float>(data[index++]);
}
