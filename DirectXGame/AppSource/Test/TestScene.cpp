#include "TestScene.h"
#include <imgui/imgui.h>
#include <Utility/ConvertString.h>
#include"Common/DebugParam/GameParamEditor.h"
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
#ifdef USE_IMGUI
	// 値の適応
	testParam_ = GameParamEditor::GetInstance()->GetValue<float>("TestGroup", "TestParam");
#endif


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

	ImGui::Begin("FontTest");
	ImGui::ColorEdit4("FontColor", &fontColor_.x);
	ImGui::InputText("InputText", willLoadPath_, 256);
	text_ = ConvertString(std::string(willLoadPath_));
	ImGui::DragFloat3("Scale", &transform_.scale.x, 0.1f);
	ImGui::DragFloat3("Rotate", &transform_.rotate.x, 0.01f);
	ImGui::DragFloat3("Position", &transform_.position.x, 0.1f);
	ImGui::End();

#endif

	engine_->ImGuiDraw();
}
