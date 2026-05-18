#include "LevelSystemUI.h"
#include <imgui/imgui.h>
#include <Utility/Easing.h>

namespace {
	Vector2 cpPoint;
}

LevelSystemUI::~LevelSystemUI() {
	Save();
}

void LevelSystemUI::Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager, SHEngine::TextureManager* textureManager) {
	render_ = std::make_unique<SHEngine::RenderObject>("LevelSystemUI");
	render_->Initialize();
	render_->psoConfig_.vs = "Game/Spline.VS.hlsl";
	render_->psoConfig_.ps = "White.PS.hlsl";
	render_->CreateSRV(sizeof(Vector2), kSegment_ + 1, ShaderType::VERTEX_SHADER, "Points");
	render_->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER, "WVP");

	static int drawDataIndex = -1;
	if (drawDataIndex == -1) {
		std::vector<Vector3> dummyVertices(2, Vector3{ 0.0f, 0.0f, 0.0f });
		drawDataManager->AddVertexBuffer(dummyVertices);
		drawDataIndex = drawDataManager->CreateDrawData();
	}

	render_->SetDrawData(drawDataManager->GetDrawData(drawDataIndex));
	render_->psoConfig_.topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
	render_->psoConfig_.depthStencilID = SHEngine::PSO::DepthStencilID::Transparent;
	render_->psoConfig_.inputLayoutID = SHEngine::PSO::InputLayoutID::Vector3;
	render_->instanceNum_ = kSegment_;

	currentPoint_ = std::make_unique<SHEngine::RenderObject>("LevelSystem::CurrentPoint");
	currentPoint_->Initialize();
	currentPoint_->psoConfig_.vs = "Simple.VS.hlsl";
	currentPoint_->psoConfig_.ps = "PostEffect/Simple.PS.hlsl";
	currentPoint_->SetUseTexture(true);
	currentPoint_->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER, "WVP");
	currentPoint_->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "TextureIndex");
	currentPoint_->SetDrawData(drawDataManager->GetDrawData(modelManager->GetNodeModelData(1).drawDataIndex));
	currentPoint_->psoConfig_.depthStencilID = SHEngine::PSO::DepthStencilID::Transparent;

	cpTextureIndex_ = textureManager->LoadTexture("PlayerIcon.png");

	cpTransform_.rotate.z = std::numbers::pi_v<float> / 2.0f;

	Load();
}

void LevelSystemUI::Update(const LevelSystem& levelSystem, Matrix4x4 vpMatrix, float deltaTime) {
	auto vertices = levelSystem.GetWaveVertices();
	std::vector<Vector2> points;
	points.reserve(vertices.size());
	for (const auto& vertex : vertices) {
		points.emplace_back(vertex.time, vertex.intensity);
	}

	vertices_ = MyMath::GetSplinePoints(points, kSegment_);

	wvpMatrix_ = Matrix::MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.position) * vpMatrix;

	render_->CopyBufferData(0, vertices_.data(), sizeof(Vector2) * vertices_.size());
	render_->CopyBufferData(1, &wvpMatrix_, sizeof(Matrix4x4));

	// 左右に回すアニメーション
	static float t = 0.0f;
	float baseRotZ = std::numbers::pi_v<float> / 2.0f;
	t += deltaTime;
	cpTransform_.rotate.z = baseRotZ + std ::sinf(t * frequency_) * amplitude_;

	cpPoint = levelSystem.GetCurrentPoint();
	cpwvpMatrix_ = Matrix::MakeAffineMatrix(cpTransform_.scale, cpTransform_.rotate, { cpPoint.x, cpPoint.y, 0.0f }) *
		Matrix::MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.position) * 
		vpMatrix;

	currentPoint_->CopyBufferData(0, &cpwvpMatrix_, sizeof(Matrix4x4));
	currentPoint_->CopyBufferData(1, &cpTextureIndex_, sizeof(int));
}

void LevelSystemUI::Draw(CmdObj* cmdObj) {
	render_->Draw(cmdObj);
	currentPoint_->Draw(cmdObj);
}

void LevelSystemUI::DrawImGui() {
#ifdef USE_IMGUI
    
	ImGui::Begin("LevelSystemUI Transform");
	ImGui::DragFloat3("Scale", &transform_.scale.x, 0.1f);
	ImGui::DragFloat3("Rotate", &transform_.rotate.x, 0.1f);
	ImGui::DragFloat3("Position", &transform_.position.x, 0.1f);

	ImGui::Separator();

	ImGui::PushID("CurrentPoint");
	ImGui::DragFloat3("Scale", &cpTransform_.scale.x, 0.1f);
	ImGui::DragFloat3("Rotate", &cpTransform_.rotate.x, 0.1f);
	ImGui::PopID();

	ImGui::Text("Current Point: (%.2f, %.2f)", cpPoint.x, cpPoint.y);

	ImGui::End();

#endif
}

void LevelSystemUI::Load() {
	binaryManager_.Boot(saveFilePath_);
	if (binaryManager_.IsEmpty()) {
		return;
	}

	transform_.scale = binaryManager_.Reverse<Vector3>();
	transform_.rotate = binaryManager_.Reverse<Vector3>();
	transform_.position = binaryManager_.Reverse<Vector3>();

	cpTransform_.scale = binaryManager_.Reverse<Vector3>();
}

void LevelSystemUI::Save() {
	binaryManager_.Boot(saveFilePath_);
	binaryManager_.Register(&transform_.scale);
	binaryManager_.Register(&transform_.rotate);
	binaryManager_.Register(&transform_.position);
	binaryManager_.Register(&cpTransform_.scale);
	binaryManager_.Write(saveFilePath_);
}
