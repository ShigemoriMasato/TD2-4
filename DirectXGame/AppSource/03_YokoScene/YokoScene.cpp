#include "YokoScene.h"
#include <numbers>
#include <algorithm>

void YokoScene::Initialize()
{
	camera_ = std::make_unique<DebugCamera>();
	camera_->SetProjectionMatrix(PerspectiveFovDesc{});
	camera_->SetPosition({ 0.0f, 8.0f, -25.0f });
	camera_->Initialize(input_);

	// トレイル初期化
	testTrail1_ = std::make_unique<TestTrail1>();
	testTrail1_->Initialize(drawDataManager_, textureManager_);

	testTrail2_ = std::make_unique<TestTrail2>();
	testTrail2_->Initialize(drawDataManager_, textureManager_);

	testTrail3_ = std::make_unique<TestTrail3>();
	testTrail3_->Initialize(drawDataManager_, textureManager_);

	testTrail4_ = std::make_unique<TestTrail4>();
	int swordModelHandle = modelManager_->LoadModel("Assets/Model/Item/Weapon/Sword");
	testTrail4_->Initialize(drawDataManager_, textureManager_, modelManager_, swordModelHandle);
}

std::unique_ptr<IScene> YokoScene::Update()
{
	const float dt = engine_->GetFPSObserver()->GetDeltatime();

	// カメラ更新
	camera_->Update();

	// トレイル更新
	testTrail1_->Update(dt, camera_->GetVPMatrix());
	testTrail2_->Update(dt, camera_->GetVPMatrix());
	testTrail3_->Update(dt, camera_->GetVPMatrix());
	testTrail4_->Update(dt, camera_->GetVPMatrix());

	const auto isPress = input_->GetKeyState(DIK_SPACE);
	if (isPress)testTrail3_->Trigger(Vector3(0.0f, 0.0f, 0.0f));

	return nullptr;
}

void YokoScene::Draw()
{
	auto window = commonData_->mainWindow.second.get();
	auto display = commonData_->display.get();
	auto cmdObj = commonData_->cmdObject.get();

	display->PreDraw(cmdObj, true);

	// トレイル描画
	//testTrail1_->Draw(cmdObj);
	//testTrail2_->Draw(cmdObj);
	//testTrail3_->Draw(cmdObj);
	testTrail4_->Draw(cmdObj);

	display->PostDraw(cmdObj);

	window->PreDraw(cmdObj);

#ifdef USE_IMGUI
	display->DrawImGui();
#endif

	engine_->DrawImGui();
	window->PostDraw(cmdObj);
}