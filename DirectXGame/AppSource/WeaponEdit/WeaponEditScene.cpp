#include "WeaponEditScene.h"

WeaponEditScene::WeaponEditScene() {
#ifdef USE_IMGUI
	auto& io = ImGui::GetIO();
	io.IniFilename = "Assets/ImGui/WeaponEdit.ini"
#endif
}

WeaponEditScene::~WeaponEditScene() {
#ifdef USE_IMGUI
	auto& io = ImGui::GetIO();
	io.IniFilename = "Assets/ImGui/imgui.ini";
#endif
}

void WeaponEditScene::Initialize() {
	weaponImGui_ = std::make_unique<WeaponImGui>(engine_);
}

std::unique_ptr<IScene> WeaponEditScene::Update() {
	weaponImGui_->Update();
	return nullptr;
}

void WeaponEditScene::Draw() {
	auto cmdObj = commonData_->cmdObject.get();
	auto display = commonData_->display.get();
	auto window = commonData_->mainWindow.second->GetCurrentDisplay();

	display->PreDraw(cmdObj);

	display->PostDraw(cmdObj);



	cmdObj->SetRenderTarget(window);

	engine_->DrawImGui();

	window->ToPresent(cmdObj);

}
