#include "LightManager.h"
#include <imgui/imgui.h>

LightManager::~LightManager() {
	Save();
}

void LightManager::Initialize() {
	container_ = std::make_unique<SHEngine::BufferContainer>();

	lightData_.directional = container_->Create(BufferType::SRV, sizeof(DirectionalLight), 16);
	lightData_.point = container_->Create(BufferType::SRV, sizeof(PointLight), 16);
	lightData_.config = container_->Create(BufferType::CBV, sizeof(Config));

	directionalLights_.resize(16);
	pointLights_.resize(16);

	Load();

	lightData_.directional->CopyBuffer(directionalLights_.data(), sizeof(DirectionalLight) * directionalLights_.size());
	lightData_.point->CopyBuffer(pointLights_.data(), sizeof(PointLight) * pointLights_.size());
	lightData_.config->CopyBuffer(&config_, sizeof(Config));
}

void LightManager::DrawImGui() {
#ifdef USE_IMGUI

	ImGui::Begin("LightManager");

	ImGui::PushID("Directional");
	ImGui::Text("Direct: %d / %d", currentDirNum_, config_.directionalNum - 1);
	ImGui::SameLine();
	if (ImGui::Button("-")) {
		currentDirNum_--;
	}
	ImGui::SameLine();
	if (ImGui::Button("+")) {
		currentDirNum_++;
	}
	currentDirNum_ = std::clamp(currentDirNum_, 0, config_.directionalNum - 1);

	if (ImGui::Button("Add")) {
		if (config_.directionalNum < 16) {
			config_.directionalNum++;
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Remove")) {
		if (config_.directionalNum > 0) {
			config_.directionalNum--;
		}
	}

	auto& currentDir = directionalLights_[currentDirNum_];

	ImGui::ColorEdit3("Color", &currentDir.color.x);
	ImGui::DragFloat3("Direction", &currentDir.direction.x, 0.1f);
	ImGui::DragFloat("Intensity", &currentDir.intensity, 0.01f, 0.0f);
	currentDir.direction = currentDir.direction.Normalize();

	ImGui::PopID();

	ImGui::Separator();

	ImGui::PushID("Point");

	ImGui::Text("Point: %d / %d", currentPointNum_, config_.pointNum - 1);
	ImGui::SameLine();
	if (ImGui::Button("-")) {
		currentPointNum_--;
	}
	ImGui::SameLine();
	if (ImGui::Button("+")) {
		currentPointNum_++;
	}
	currentPointNum_ = std::clamp(currentPointNum_, 0, config_.pointNum - 1);

	if (ImGui::Button("Add")) {
		if (config_.pointNum < 16) {
			config_.pointNum++;
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Remove")) {
		if (config_.pointNum > 0) {
			config_.pointNum--;
		}
	}

	auto& currentPoint = pointLights_[currentPointNum_];

	ImGui::ColorEdit3("Color", &currentPoint.color.x);
	ImGui::DragFloat3("Position", &currentPoint.position.x, 0.01f);
	ImGui::DragFloat("Intensity", &currentPoint.intensity, 0.01f, 0.0f);
	ImGui::DragFloat("Radius", &currentPoint.radius, 0.01f, 0.0f);
	ImGui::DragFloat("Decay", &currentPoint.decay, 0.01f, 0.0f);

	ImGui::PopID();

	ImGui::End();

	lightData_.directional->CopyBuffer(directionalLights_.data(), sizeof(DirectionalLight) * directionalLights_.size());
	lightData_.point->CopyBuffer(pointLights_.data(), sizeof(PointLight) * pointLights_.size());
	lightData_.config->CopyBuffer(&config_, sizeof(Config));

#endif // USE_IMGUI

}

void LightManager::Save() {
	BinaryManager bin;
	const std::string saveFile = "IntroLightData.bin";

	bin.Register<int>(&config_.directionalNum);
	for (int i = 0; i < config_.directionalNum; ++i) {
		bin.Register<DirectionalLight>(&directionalLights_[i]);
	}
	bin.Register<int>(&config_.pointNum);
	for (int i = 0; i < config_.pointNum; ++i) {
		bin.Register<PointLight>(&pointLights_[i]);
	}

	bin.Write(saveFile);
}

void LightManager::Load() {
	BinaryManager bin;
	const std::string saveFile = "IntroLightData.bin";
	if (!bin.Boot(saveFile)) {
		return;
	}

	config_.directionalNum = bin.Reverse<int>();
	for (int i = 0; i < config_.directionalNum; ++i) {
		directionalLights_[i] = bin.Reverse<DirectionalLight>();
	}

	config_.pointNum = bin.Reverse<int>();
	for (int i = 0; i < config_.pointNum; ++i) {
		pointLights_[i] = bin.Reverse<PointLight>();
	}
}
