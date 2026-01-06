#include "WaterWave.h"
#include <imgui/imgui.h>

WaterWave::~WaterWave() {
	Save();
}

void WaterWave::Initialize(const DrawData& drawData, Camera* camera) {
	camera_ = camera;
	renderObject_ = std::make_unique<RenderObject>("WaterWave");
	renderObject_->Initialize();
	renderObject_->psoConfig_.vs = "Water/WaterPlane.VS.hlsl";
	renderObject_->psoConfig_.ps = "Water/WaterPlane.PS.hlsl";
	renderObject_->SetDrawData(drawData);
	renderObject_->CreateCBV(sizeof(UniqueData), ShaderType::VERTEX_SHADER, "WaterWave::UniqueData");
	renderObject_->CreateCBV(sizeof(DirectionalLight), ShaderType::PIXEL_SHADER, "WaterWave::DirectionalLight");
	renderObject_->CreateCBV(sizeof(Vector4), ShaderType::PIXEL_SHADER, "WaterWave::BaseColor");
	renderObject_->instanceNum_ = 1;
	binaryManager_ = std::make_unique<BinaryManager>();
	Load();

	uniqueData_.world = Matrix::MakeAffineMatrix(scale, rotation, position);
}

void WaterWave::Update(float deltaTime) {
	uniqueData_.time += deltaTime;
	uniqueData_.cameraPos = camera_->position_;
	uniqueData_.waveCount = 0;
	uniqueData_.vp = camera_->GetVPMatrix();
	for (int i = 0; i < 8; ++i) {
		uniqueData_.waves[i] = waves_[i];
		if (waves_[i].amplitude == 0.0f && waves_[i].wavelength == 0.0f) {
			continue;
		}
		uniqueData_.waveCount++;
	}
}

void WaterWave::Draw(Window* window) {

	renderObject_->CopyBufferData(0, &uniqueData_, sizeof(UniqueData));
	renderObject_->CopyBufferData(1, &light_, sizeof(DirectionalLight));
	renderObject_->CopyBufferData(2, &baseColor_, sizeof(Vector4));

	renderObject_->Draw(window);
}

void WaterWave::DrawImGui() {
#ifdef USE_IMGUI
	ImGui::Begin("WaterPlaneResource");

	time_ += 0.0166666667f; //約60fps

	ImGui::ColorEdit4("Light Color", &light_.color.x);
	ImGui::DragFloat3("Light Direction", &light_.direction.x, 0.01f);
	light_.direction = light_.direction.Normalize();
	ImGui::DragFloat("Light Intensity", &light_.intensity, 0.1f);

	ImGui::Separator();

	ImGui::DragFloat3("Scale", &scale.x, 0.1f, 0.1f);
	ImGui::DragFloat3("Rotation", &rotation.x, 0.1f);
	ImGui::DragFloat3("Position", &position.x, 0.1f);
	ImGui::ColorEdit4("Base Color", &baseColor_.x);
	uniqueData_.world = Matrix::MakeAffineMatrix(scale, rotation, position);

	ImGui::Separator();

	static int currentWave = 0;
	ImGui::Text("%d", currentWave);
	ImGui::SameLine();
	if (ImGui::Button("+")) {
		currentWave++;
	}
	ImGui::SameLine();
	if (ImGui::Button("-")) {
		currentWave--;
	}
	currentWave = std::clamp(currentWave, 0, static_cast<int>(waves_.size() - 1));

	ImGui::DragFloat2("Direction", &waves_[currentWave].direction.x, 0.1f, -1.0f, 1.0f);
	waves_[currentWave].direction = waves_[currentWave].direction.Normalize();
	ImGui::DragFloat("Amplitude", &waves_[currentWave].amplitude, 0.01f, 0.0f, 1000.0f);
	ImGui::DragFloat("Wavelength", &waves_[currentWave].wavelength, 0.1f, 0.1f, 100.0f);
	ImGui::DragFloat("Speed", &waves_[currentWave].speed, 0.1f, 0.0f, 1000.0f);

	ImGui::End();

	for (int i = 0; i < 8; ++i) {
		uniqueData_.waves[i] = waves_[i];
		if (waves_[i].amplitude == 0.0f && waves_[i].wavelength == 0.0f) {
			continue;
		}
		uniqueData_.waveCount++;
	}
#endif
}

void WaterWave::Save() {
	binaryManager_->RegistOutput(scale);
	binaryManager_->RegistOutput(rotation);
	binaryManager_->RegistOutput(position);
	binaryManager_->RegistOutput(int(waves_.size()));
	for (size_t i = 0; i < waves_.size(); i++) {
		binaryManager_->RegistOutput(waves_[i].direction.x);
		binaryManager_->RegistOutput(waves_[i].direction.y);
		binaryManager_->RegistOutput(waves_[i].amplitude);
		binaryManager_->RegistOutput(waves_[i].wavelength);
		binaryManager_->RegistOutput(waves_[i].speed);
	}
	binaryManager_->RegistOutput(light_.color);
	binaryManager_->RegistOutput(light_.direction);
	binaryManager_->RegistOutput(light_.intensity);
	binaryManager_->RegistOutput(baseColor_);
	binaryManager_->Write("WaterPlaneResource.bin");
}

void WaterWave::Load() {
	//初期値設定
	auto data = binaryManager_->Read("WaterPlaneResource.bin");
	size_t index = 0;
	if (!data.empty()) {
		scale = binaryManager_->Reverse<Vector3>(data[index++]);
		rotation = binaryManager_->Reverse<Vector3>(data[index++]);
		position = binaryManager_->Reverse<Vector3>(data[index++]);
		int waveSize = binaryManager_->Reverse<int>(data[index++]);
		waves_.resize(waveSize);
		for (size_t i = 0; i < waves_.size(); i++) {
			waves_[i].direction.x = binaryManager_->Reverse<float>(data[index++]);
			waves_[i].direction.y = binaryManager_->Reverse<float>(data[index++]);
			waves_[i].amplitude = binaryManager_->Reverse<float>(data[index++]);
			waves_[i].wavelength = binaryManager_->Reverse<float>(data[index++]);
			waves_[i].speed = binaryManager_->Reverse<float>(data[index++]);
		}
		light_.color = binaryManager_->Reverse<Vector4>(data[index++]);
		light_.direction = binaryManager_->Reverse<Vector3>(data[index++]);
		light_.intensity = binaryManager_->Reverse<float>(data[index++]);
	} else {
		waves_.resize(8);
	}

	if(data.size() <= index) {
		return;
	}

	baseColor_ = binaryManager_->Reverse<Vector4>(data[index++]);
}
