#include "BackPackBuildScene.h"
#include "GameObject/BackPack/BackPack.h"
#include "GameObject/BackPack/Shop/Shop.h"
#include "GameObject/Item/ItemManager.h"

BackPackBuildScene::BackPackBuildScene()
{
	camera_ = std::make_unique<DebugCamera>();
	grid_ = std::make_unique<Grid>();

	itemManager_ = std::make_unique<ItemManager>();
	backPack_ = std::make_unique<BackPack>();
	shop_ = std::make_unique<Shop>();
}

BackPackBuildScene::~BackPackBuildScene()
{}

void BackPackBuildScene::Initialize()
{
	camera_->Initialize(input_);
	grid_->Initialize(drawDataManager_);

	itemManager_->Initialize(modelManager_);
	backPack_->Initialize(modelManager_, drawDataManager_, itemManager_.get(), commonData_, input_);
	shop_->Initialize(modelManager_, drawDataManager_, itemManager_.get(), commonData_, input_, backPack_.get());
}

std::unique_ptr<IScene> BackPackBuildScene::Update()
{
	// カメラの更新
	camera_->Update();
	// グリッド
	grid_->Update(camera_->GetCenter(), camera_->GetVPMatrix());

	// ショップ
	shop_->Update(camera_->GetVPMatrix());
	// バックパック
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
	shop_->Draw(cmdObj);

	display->PostDraw(cmdObj);


	window->PreDraw(cmdObj);

	//ここ以外で記述する場合、ifdefを忘れないようにすること
#ifdef USE_IMGUI

	display->DrawImGui();

	backPack_->DrawImGui();

	itemManager_->DrawImGui();

	engine_->DrawImGui();
#endif

	window->PostDraw(cmdObj);
}
