#include "YokoyamaScene.h"
#include <imgui/imgui.h>
#include <Utility/Color.h>
#include <Game/Item/ItemManager.h>

YokoyamaScene::YokoyamaScene()
{
	itemManager_ = std::make_unique<ItemManager>();

}

YokoyamaScene::~YokoyamaScene()
{}

void YokoyamaScene::Initialize()
{
	itemManager_->Initialize(modelManager_);
}

std::unique_ptr<IScene> YokoyamaScene::Update()
{
	return nullptr;
}

void YokoyamaScene::Draw()
{
	auto window = commonData_->mainWindow.second.get();
	auto display = commonData_->display.get();
	auto cmdObj = commonData_->cmdObject.get();

	display->PreDraw(cmdObj, true);
	display->PostDraw(cmdObj);

	window->PreDraw(cmdObj);

	//ここ以外で記述する場合、ifdefを忘れないようにすること
#ifdef USE_IMGUI

	display->DrawImGui();

	itemManager_->DrawImGui();

	ImGui::Begin("FPS");
	float deltaTime = engine_->GetFPSObserver()->GetDeltatime();
	ImGui::Text("DeltaTime: %.3f ms", deltaTime * 1000.0f);
	ImGui::Text("FPS: %.1f", 1.0f / deltaTime);
	ImGui::End();

#endif


	engine_->DrawImGui();
	window->PostDraw(cmdObj);
}
