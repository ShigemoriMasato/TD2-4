#include "BackPackBuildScene.h"
#include "GameObject/BackPack/BackPack.h"
#include "GameObject/Item/ItemManager.h"

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
	grid_->Initialize(drawDataManager_);
	itemManager_->Initialize(modelManager_);
	camera_->Initialize(input_);
	backPack_->Initialize(modelManager_, drawDataManager_, itemManager_.get(), commonData_);
}

std::unique_ptr<IScene> BackPackBuildScene::Update()
{
	// カメラの更新
	camera_->Update();

	// グリッド
	grid_->Update(camera_->GetCenter(), camera_->GetVPMatrix());
	
	// ビルドシーンのメイン
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


	window->PreDraw(cmdObj);

	display->PostDraw(cmdObj);

	//ここ以外で記述する場合、ifdefを忘れないようにすること
#ifdef USE_IMGUI

	display->DrawImGui();

	backPack_->DrawImGui();

	itemManager_->DrawImGui();

	ImGui::Begin("FPS");
	float deltaTime = engine_->GetFPSObserver()->GetDeltatime();
	ImGui::Text("DeltaTime: %.3f ms", deltaTime * 1000.0f);
	ImGui::Text("FPS: %.1f", 1.0f / deltaTime);
	ImGui::End();

	ImGui::Begin("Depth");
	auto depthTexture = display->GetDisplay()->GetDepthTexture();
	ImGui::Image(ImTextureRef(depthTexture->GetGPUHandle().ptr), { 640, 360 });
	ImGui::End();

	engine_->DrawImGui();
#endif

	window->PostDraw(cmdObj);
}
