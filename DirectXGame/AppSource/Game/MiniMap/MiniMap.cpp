#include "MiniMap.h"
#include <numbers>

void MiniMap::Initialize(int mapWidth, int mapHeight, TextureManager* textureManager, const DrawData& plane, const DrawData& visionField) {
	// カメラの初期化
	camera_ = std::make_unique<Camera>();
	camera_->SetProjectionMatrix(PerspectiveFovDesc());

	transform_.rotate.x = std::numbers::pi_v<float> / 2.0f;
	distRatio_ = 0.7f;

	dist = std::max(mapWidth, mapHeight) * 0.5f / std::tan(0.45f / 2.0f);
	transform_.position = { mapWidth / 2.0f - 0.5f, 0.0f, mapHeight / 2.0f - 0.5f };
	Vector3 cameraPos = transform_.position + Vector3{ 0.0f, dist * distRatio_, 0.0f };
	camera_->SetTransform(Matrix::MakeAffineMatrix(transform_.scale, transform_.rotate, {cameraPos}));
	camera_->MakeMatrix();
	// デュアルディスプレイの初期化
	display_ = std::make_unique<DualDisplay>("MiniMapDisplay");
	int textureHandle1 = textureManager->CreateWindowTexture(1280, 720, 0xffffffff);
	int textureHandle2 = textureManager->CreateWindowTexture(1280, 720, 0xffffffff);
	display_->Initialize(textureManager->GetTextureData(textureHandle1), textureManager->GetTextureData(textureHandle2));

	miniMapRender_ = std::make_unique<RenderObject>();
	miniMapRender_->Initialize();
	miniMapRender_->SetDrawData(plane);
	miniMapRender_->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER, "Matrix");
	miniMapRender_->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "TextureIndex");
	miniMapRender_->SetUseTexture(true);
	miniMapRender_->psoConfig_.vs = "Simple.VS.hlsl";
	miniMapRender_->psoConfig_.ps = "PostEffect/Simple.PS.hlsl";

	visionField_ = std::make_unique<RenderObject>();
	visionField_->Initialize();
	visionField_->SetDrawData(visionField);
	visionField_->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER, "WVP");
	visionField_->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "TextureIndex");
	visionField_->SetUseTexture(true);
	visionField_->psoConfig_.vs = "Simple.VS.hlsl";
	visionField_->psoConfig_.ps = "PostEffect/Simple.PS.hlsl";
	vfModelsTextureIndex_ = textureManager->LoadTexture("Mineral-0.png");
}

Camera* MiniMap::PreDraw(Window* window) {
	display_->PreDraw(window->GetCommandObject(), true);
	return camera_.get();
}

void MiniMap::PostDraw(Window* window, const Matrix4x4& vpMatrix, Vector3 playerPosition, float range) {
	//枠を描画
	float scale = range / rangeAdjust_;
	Vector3 pos = { playerPosition.x, 0.1f, playerPosition.z };
	Matrix4x4 mat = Matrix::MakeAffineMatrix({scale, scale, scale }, {}, pos) * vpMatrix;
	visionField_->CopyBufferData(0, &mat, sizeof(Matrix4x4));
	visionField_->CopyBufferData(1, &vfModelsTextureIndex_, sizeof(int));
	visionField_->Draw(window);

	display_->PostDraw(window->GetCommandObject());
}

void MiniMap::Draw(Window* window) {
	Matrix4x4 matWorld = Matrix::MakeAffineMatrix({ 0.5f, 0.5f, 0.0f }, {0.0f, 0.0f, 1.57f}, { 0.75f, -0.75f, 0.0f });
	miniMapRender_->CopyBufferData(0, &matWorld, sizeof(Matrix4x4));
	int textureIndex = display_->GetTextureData()->GetOffset();
	miniMapRender_->CopyBufferData(1, &textureIndex, sizeof(int));

	miniMapRender_->Draw(window);
}

void MiniMap::DrawImGui() {
#ifdef USE_IMGUI

	ImGui::Begin("MiniMap");

	TextureData* textureData = display_->GetTextureData();
	ImGui::Image((void*)(intptr_t)textureData->GetGPUHandle().ptr, ImVec2(400, 225));
	ImGui::DragFloat("Distance Ratio", &distRatio_, 0.01f);
	ImGui::DragFloat3("Scale", &transform_.scale.x, 0.1f);
	ImGui::DragFloat3("Rotate", &transform_.rotate.x, 0.1f);
	ImGui::DragFloat3("Position", &transform_.position.x, 0.1f);
	Vector3 cameraPos = transform_.position + Vector3{ 0.0f, dist * distRatio_, 0.0f };
	camera_->SetTransform(Matrix::MakeAffineMatrix(transform_.scale, transform_.rotate, { cameraPos }).Inverse());
	camera_->MakeMatrix();

	ImGui::End();

#endif
}
