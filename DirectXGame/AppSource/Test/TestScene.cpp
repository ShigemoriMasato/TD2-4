#include "TestScene.h"
#include <imgui/imgui.h>

void TestScene::Initialize() {
}

std::unique_ptr<IScene> TestScene::Update() {
	ImGui::Begin("Test Scene");
	ImGui::Text("This is a test scene.");
	ImGui::End();

	return nullptr;
}

void TestScene::Draw() {
	commonData_->mainWindow->PreDraw();



	commonData_->mainWindow->PostDraw();
	engine_->ImGuiDraw();
}
