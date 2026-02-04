#include "GameOverTask.h"
#include <Utility/Easing.h>

void GameOverTask::Initialize(const DrawData& drawData, int textureIndex, float* fade) {
	render_ = std::make_unique<RenderObject>();
	render_->Initialize();
	render_->SetDrawData(drawData);
	render_->CreateSRV(sizeof(VSData), 4, ShaderType::VERTEX_SHADER, "vsData");
	render_->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "textureIndex");
	render_->SetUseTexture(true);
	render_->psoConfig_.vs = "GameOver.VS.hlsl";
	render_->psoConfig_.ps = "GameOver.PS.hlsl";
	render_->instanceNum_ = tieNum_;

	vsData_.resize(tieNum_);
	transforms_.resize(tieNum_);
	uvTransforms_.resize(tieNum_);

	textureIndex_ = textureIndex;
	fade_ = fade;

	backGround_ = std::make_unique<RenderObject>();
	backGround_->Initialize();
	backGround_->SetDrawData(drawData);
	backGround_->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER, "wvpMatrix");
	backGround_->CreateCBV(sizeof(Vector4), ShaderType::PIXEL_SHADER, "color");
	backGround_->psoConfig_.vs = "Simple.VS.hlsl";
	backGround_->psoConfig_.ps = "Color.PS.hlsl";

	Load();
}

bool GameOverTask::Update(float deltaTime) {
	if (debug_) {
		timer_ += deltaTime;
	}

	for (int i = 0; i < tieNum_; ++i) {
		float offset = float(i) * 0.15f;
		vsData_[i].progress = std::min(1.0f, timer_ + offset);
		const Transform& trans = transforms_[i];
		vsData_[i].scale = lerp(Vector3(0.2f, 0.0f, 0.2f), Vector3(1.0f, 1.0f, 1.0f), vsData_[i].progress, EaseType::EaseOutElastic) * trans.scale;
		vsData_[i].worldMatrix = Matrix::MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, trans.rotate, trans.position);
		vsData_[i].uvMatrix = Matrix::MakeAffineMatrix(uvTransforms_[i].scale, uvTransforms_[i].rotate, uvTransforms_[i].position);
	}

	alpha_ = timer_ / 1.5f;

	*fade_ = std::clamp((timer_ - 2.0f) * 1.3f, 0.0f, 1.0f);

	if(timer_ > duration_) {
		return true;
	}

	return false;
}

void GameOverTask::Draw(Window* window, const Matrix4x4& vpMat) {
	for(auto& data : vsData_) {
		data.vpMatrix = vpMat;
	}

	Matrix4x4 bgMat = Matrix4x4::Identity();
	Vector4 color = { 0.0f, 0.0f, 0.0f, alpha_ };
	backGround_->CopyBufferData(0, &bgMat, sizeof(Matrix4x4));
	backGround_->CopyBufferData(1, &color, sizeof(Vector4));
	backGround_->Draw(window);

	render_->CopyBufferData(0, vsData_.data(), sizeof(VSData) * vsData_.size());
	render_->CopyBufferData(1, &textureIndex_, sizeof(int));
	render_->Draw(window);
}

void GameOverTask::DrawImGui() {
#ifdef USE_IMGUI
	ImGui::Begin("GameOverTask");
	ImGui::DragFloat("Timer", &timer_, 0.01f, 0.0f, duration_);
	
	static int index;
	for (int i = 0; i < 4; ++i) {
		std::string label = "Tie " + std::to_string(i);
		if (ImGui::Selectable(label.c_str(), index == i)) {
			index = i;
		}
	}

	Transform& trans = transforms_[index];
	Transform& uv = uvTransforms_[index];
	ImGui::DragFloat3("Scale", &trans.scale.x, 1.0f);
	ImGui::DragFloat3("Rotation", &trans.rotate.x, 0.01f);
	ImGui::DragFloat3("Position", &trans.position.x, 1.0f);
	ImGui::DragFloat3("UVScale", &uv.scale.x, 0.01f, 0.1f);
	ImGui::DragFloat3("UVRotation", &uv.rotate.x, 0.01f);
	ImGui::DragFloat3("UVPosition", &uv.position.x, 0.1f);
	if (ImGui::Button("Toggle")) {
		debug_ = !debug_;
	}

	ImGui::End();
#endif
}

void GameOverTask::Save() {
	for (int i = 0; i < tieNum_; ++i) {
		const auto& trans = transforms_[i];
		const auto& uv = uvTransforms_[i];
		bManager_.RegisterOutput(trans.scale);
		bManager_.RegisterOutput(trans.rotate);
		bManager_.RegisterOutput(trans.position);
		bManager_.RegisterOutput(uv.scale);
		bManager_.RegisterOutput(uv.rotate);
		bManager_.RegisterOutput(uv.position);
	}
	bManager_.Write(name_);
}

void GameOverTask::Load() {
	auto values = bManager_.Read(name_);
	if (values.empty()) {
		return;
	}

	int index = 0;
	for (int i = 0; i < tieNum_; ++i) {
		auto& trans = transforms_[i];
		auto& uv = uvTransforms_[i];
		trans.scale = bManager_.Reverse<Vector3>(values[index++].get());
		trans.rotate = bManager_.Reverse<Vector3>(values[index++].get());
		trans.position = bManager_.Reverse<Vector3>(values[index++].get());
		uv.scale = bManager_.Reverse<Vector3>(values[index++].get());
		uv.rotate = bManager_.Reverse<Vector3>(values[index++].get());
		uv.position = bManager_.Reverse<Vector3>(values[index++].get());
	}
}
