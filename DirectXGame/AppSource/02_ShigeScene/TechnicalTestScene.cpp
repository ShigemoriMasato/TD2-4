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
}

std::unique_ptr<IScene> TechnicalTestScene::Update() {
	debugCamera_->Update();
	text_->Update(debugCamera_->GetVPMatrix());
	text_->SetTransform(textTransform_);

	return nullptr;
}

void TechnicalTestScene::Draw() {

	auto window = commonData_->mainWindow.second.get();
	auto display = commonData_->display.get();
	auto cmdObj = commonData_->cmdObject.get();

	display->PreDraw(cmdObj, true);

	text_->Draw(cmdObj);

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
