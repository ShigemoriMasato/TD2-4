#include "TitleScene.h"
#include <imgui/imgui.h>
#include <Utility/Color.h>

TitleScene::TitleScene() {
}

void TitleScene::Initialize() {
}

std::unique_ptr<IScene> TitleScene::Update() {
	// ZキーでCharaSelectSceneに遷移
	if (input_->GetKeyState(DIK_Z) && !input_->GetPreKeyState(DIK_Z)) {
		//return std::make_unique<AsakawaScene>();
	}
	
	return nullptr;
}

void TitleScene::Draw() {
	auto window = commonData_->mainWindow.second.get();
	auto display = commonData_->display.get();
	auto cmdObj = commonData_->cmdObject.get();

	display->PreDraw(cmdObj, true);
	display->PostDraw(cmdObj);

	window->PreDraw(cmdObj);

	//ここ以外で記述する場合、ifdefを忘れないようにすること
#ifdef USE_IMGUI


	display->DrawImGui();

	ImGui::Begin("FPS");
	float deltaTime = engine_->GetFPSObserver()->GetDeltatime();
	ImGui::Text("DeltaTime: %.3f ms", deltaTime * 1000.0f);
	ImGui::Text("FPS: %.1f", 1.0f / deltaTime);
	ImGui::End();
#endif

	engine_->DrawImGui();
	window->PostDraw(cmdObj);
}
