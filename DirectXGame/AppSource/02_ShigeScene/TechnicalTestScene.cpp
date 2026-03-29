#include "TechnicalTestScene.h"

using namespace SHEngine;

void TechnicalTestScene::Initialize() {
	DrawData data = drawDataManager_->GetDrawData(modelManager_->GetNodeModelData(1).drawDataIndex);
	text_ = std::make_unique<Text>();
	text_->Initialize(data, "YDWbananaslipplus.otf", 196);
	text_->SetText(L"Hello, World!");

	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize(input_);
	text_->SetColor({ 1.0f, 1.0f, 0.0f, 1.0f });

	auto tableModelData = modelManager_->GetNodeModelData(3);
	auto tableData = drawDataManager_->GetDrawData(tableModelData.drawDataIndex);
	renderer_ = std::make_unique<Renderer>(tableData);
	ResourceDesc desc{};
	desc.bufferType = uint8_t(BufferType::CBV);
	desc.elementCount = 1;
	desc.sizeInBytes = sizeof(Matrix4x4);
	gpuBuffers_.reserve(2);
	auto& wvp = gpuBuffers_.emplace_back(std::make_unique<GPUBuffer>(desc));

	desc.sizeInBytes = sizeof(int);
	auto& textureIndex = gpuBuffers_.emplace_back(std::make_unique<GPUBuffer>(desc));

	renderer_->SetGPUBuffer(wvp.get(), ShaderType::VERTEX_SHADER, BufferType::CBV);
	renderer_->SetGPUBuffer(textureIndex.get(), ShaderType::PIXEL_SHADER, BufferType::CBV);

	renderer_->isUseTexture_ = true;
	renderer_->vs_ = "Simple.VS.hlsl";
	renderer_->ps_ = "PostEffect/Simple.PS.hlsl";

	gpuBuffers_[1]->CopyBuffer(&tableModelData.materials[tableModelData.materialIndex.front()].textureIndex, sizeof(int));
}

std::unique_ptr<IScene> TechnicalTestScene::Update() {
	debugCamera_->Update();
	text_->Update(debugCamera_->GetVPMatrix());
	text_->SetTransform(textTransform_);

	Matrix4x4 wvpMat = debugCamera_->GetVPMatrix();
	auto tableModelData = modelManager_->GetNodeModelData(3);

	gpuBuffers_[0]->CopyBuffer(&wvpMat, sizeof(Matrix4x4));

	return nullptr;
}

void TechnicalTestScene::Draw() {

	auto window = commonData_->mainWindow.second.get();
	auto display = commonData_->display.get();
	auto cmdObj = commonData_->cmdObject.get();

	display->PreDraw(cmdObj, true);

	text_->Draw(cmdObj);
	renderer_->Draw(cmdObj);

	display->PostDraw(cmdObj);

	window->PreDraw(cmdObj);

#ifdef USE_IMGUI

	ImGui::Begin("Text");
	ImGui::DragFloat3("Scale", &textTransform_.scale.x, 0.01f);
	ImGui::DragFloat3("Rotate", &textTransform_.rotate.x, 0.01f);
	ImGui::DragFloat3("Position", &textTransform_.position.x, 0.01f);
	ImGui::End();

#endif

	display->DrawImGui();

	engine_->DrawImGui();

	window->PostDraw(cmdObj);

}
