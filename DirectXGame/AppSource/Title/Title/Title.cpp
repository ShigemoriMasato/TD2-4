#include "Title.h"
#include <imgui/imgui.h>

Title::~Title() {
	Save();
}

void Title::Initialize(DrawData& drawData, Camera* camera) {
	renderObject_ = std::make_unique<RenderObject>("Title");
	renderObject_->Initialize();
	renderObject_->psoConfig_.vs = "Game/Block.VS.hlsl";
	renderObject_->psoConfig_.ps = "Game/Block.PS.hlsl";
	renderObject_->SetDrawData(drawData);
	renderObject_->CreateSRV(sizeof(VSData), 1, ShaderType::VERTEX_SHADER, "Title::VSData");
	renderObject_->instanceNum_ = 1;
	camera_ = camera;

	isLighting_ = false;
	binaryManager_ = std::make_unique<BinaryManager>();

	Load();
}

void Title::Update(float deltaTime) {
	time_ += deltaTime;
	if (time_ > toggleTime_) {
		isLighting_ = !isLighting_;
		time_ = 0.0f;
		toggleTime_ = float(rand() % 1000) / 200.0f + 0.3f;
	}

	data.world = Matrix::MakeScaleMatrix(scale_) * Matrix::MakeTranslationMatrix(position_);
	data.vp = camera_->GetVPMatrix();
	data.color = isLighting_ ? lightColor_ : darkColor_;
	data.outlineColor = outlineColor_;

	renderObject_->CopyBufferData(0, &data, sizeof(VSData));
}

void Title::Draw(Window* window) {
	renderObject_->Draw(window);
}

void Title::DrawImGui() {
#ifdef USE_IMGUI
	ImGui::Begin("TitleResource");
	Vector4 colorBuffer = ConvertColor(lightColor_);
	ImGui::ColorEdit4("Light Color", &colorBuffer.x);
	lightColor_ = ConvertColor(colorBuffer);
	colorBuffer = ConvertColor(darkColor_);
	ImGui::ColorEdit4("Dark Color", &colorBuffer.x);
	darkColor_ = ConvertColor(colorBuffer);
	colorBuffer = ConvertColor(outlineColor_);
	ImGui::ColorEdit4("Outline Color", &colorBuffer.x);
	outlineColor_ = ConvertColor(colorBuffer);

	ImGui::DragFloat3("Scale", &scale_.x, 0.1f, 0.1f);
	ImGui::DragFloat3("Position", &position_.x, 0.1f);
	ImGui::End();
#endif
}

void Title::Save() {
	binaryManager_->RegistOutput(scale_);
	binaryManager_->RegistOutput(position_);
	binaryManager_->RegistOutput(lightColor_);
	binaryManager_->RegistOutput(darkColor_);
	binaryManager_->RegistOutput(outlineColor_);
	binaryManager_->Write("Title.sg");
}

void Title::Load() {
	//初期値設定
	auto data = binaryManager_->Read("Title.sg");
	if (data.empty()) {
		return;
	}

	size_t index = 0;

	scale_ = binaryManager_->Reverse<Vector3>(data[index++]);
	position_ = binaryManager_->Reverse<Vector3>(data[index++]);
	lightColor_ = binaryManager_->Reverse<uint32_t>(data[index++]);
	darkColor_ = binaryManager_->Reverse<uint32_t>(data[index++]);
	outlineColor_ = binaryManager_->Reverse<uint32_t>(data[index++]);
}
