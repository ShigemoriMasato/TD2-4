#include "TestScene.h"
#include <imgui/imgui.h>
#include <Utility/ConvertString.h>
#include <Utility/Easing.h>

namespace {
	bool debug = false;

	std::string simpleSkin = "Assets/.EngineResource/Model/SimpleSkin";
	std::string sneakWalk = "SneakWalk";
}

void TestScene::Initialize() {
	
	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize();

}

std::unique_ptr<IScene> TestScene::Update() {

	return nullptr;
}

void TestScene::Draw() {
	auto window = commonData_->mainWindow.get();
	auto display = commonData_->display.get();

	display->PreDraw(window->GetCommandObject(), true);
	
	display->PostDraw(window->GetCommandObject());

	window->PreDraw(true);
	window->DrawDisplayWithImGui();

	//ImGui
#ifdef USE_IMGUI

#endif

	engine_->ImGuiDraw();
}
