#include "BackPackBuildScene.h"
#include "BackPackBuildScene/BackPack/BackPack.h"
#include <Game/Item/ItemManager.h>

BackPackBuildScene::BackPackBuildScene()
{
	itemManager_ = std::make_unique<ItemManager>();
	camera_ = std::make_unique<DebugCamera>();
	backPack_ = std::make_unique<BackPack>();
	grid_ = std::make_unique<Grid>();
}

BackPackBuildScene::~BackPackBuildScene()
{}

void BackPackBuildScene::Initialize()
{
	int modelHandle = modelManager_->LoadModel("Assets/.EngineResource/Model/Cube");
	auto modelData = modelManager_->GetNodeModelData(modelHandle);
	auto drawData = drawDataManager_->GetDrawData(modelData.drawDataIndex);

	grid_->Initialize(drawDataManager_);
	itemManager_->Initialize(modelManager_);
	camera_->Initialize(input_);
	backPack_->Initialize(modelManager_, drawDataManager_);
}

std::unique_ptr<IScene> BackPackBuildScene::Update()
{
	camera_->Update();
	Vector3 cameraPos = camera_->GetCenter();

	grid_->Update(cameraPos, camera_->GetVPMatrix());
	backPack_->Update(camera_->GetVPMatrix());

	return nullptr;
}

void BackPackBuildScene::Draw()
{
	auto window = commonData_->mainWindow.second.get();
	auto display = commonData_->display.get();
	auto cmdObj = commonData_->cmdObject.get();

	display->PreDraw(cmdObj, true);

	grid_->Draw(cmdObj);
	backPack_->Draw(cmdObj);

	display->PostDraw(cmdObj);

	window->PreDraw(cmdObj);


	//ここ以外で記述する場合、ifdefを忘れないようにすること
#ifdef USE_IMGUI

	display->DrawImGui();

	backPack_->DrawImGui();

	engine_->DrawImGui();
#endif

	window->PostDraw(cmdObj);
}
