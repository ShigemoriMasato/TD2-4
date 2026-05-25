#include "WeaponEditScene.h"

WeaponEditScene::WeaponEditScene() {
#ifdef USE_IMGUI
	auto& io = ImGui::GetIO();
	io.IniFilename = "Assets/ImGui/WeaponEdit.ini";
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
	weapon_ = std::make_unique<Weapon>();
	weaponRender_ = std::make_unique<WeaponRender>(engine_);
}

std::unique_ptr<IScene> WeaponEditScene::Update() {
	float deltaTime = 0.0f;
	if (!isStop_) {
		deltaTime = engine_->GetDeltaTime();
	}

	weaponImGui_->Update();
	weapon_->SetWeaponData(weaponImGui_->GetData());
	weapon_->Update(deltaTime);

	if (weaponImGui_->IsDataChanged()) {
		weaponRender_->DeleteRenderer(currentRenderID_);
		currentRenderID_ = weaponRender_->AddRenderData(weapon_->GetRenderData());
	}

	return nullptr;
}

void WeaponEditScene::Draw() {
	auto cmdObj = commonData_->cmdObject.get();
	auto display = commonData_->display.get();
	auto window = commonData_->mainWindow.second->GetCurrentDisplay();

	display->PreDraw(cmdObj);

	weaponRender_->Draw(cmdObj);

	display->PostDraw(cmdObj);

	display->DrawImGui();

	cmdObj->SetRenderTarget(window);

	engine_->DrawImGui();

	window->ToPresent(cmdObj);

}
