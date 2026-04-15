#include "TechnicalTestScene.h"

using namespace SHEngine;

void TechnicalTestScene::Initialize() {

	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize(input_);

}

std::unique_ptr<IScene> TechnicalTestScene::Update() {
	debugCamera_->Update();
	return nullptr;
}

void TechnicalTestScene::Draw() {

	auto window = commonData_->mainWindow.second.get();
	auto display = commonData_->display.get();
	auto cmdObj = commonData_->cmdObject.get();

	display->PreDraw(cmdObj, true);



	display->ToPresent(cmdObj);

	window->PreDraw(cmdObj);

#ifdef USE_IMGUI

#endif

	display->DrawImGui();

	engine_->DrawImGui();

	window->ToPresent(cmdObj);

}
