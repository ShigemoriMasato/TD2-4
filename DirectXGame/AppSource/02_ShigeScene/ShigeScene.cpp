#include "ShigeScene.h"
#include <imgui/imgui.h>
#include <Utility/Color.h>
#include "ShopScene.h"

void ShigeScene::Initialize() {
	camera_ = std::make_unique<DebugCamera>();
	camera_->Initialize(input_);

	grid_ = std::make_unique<Grid>();
	grid_->Initialize(drawDataManager_);

	colliderManager_ = std::make_unique<ColliderManager>();
	Collider::SetColliderManager(colliderManager_.get());

	player_ = std::make_unique<Player>();
	player_->Initialize(commonData_->keyManager.get());
	
	enemyManager_ = std::make_unique<EnemyManager>();
	enemyManager_->Initialize(player_->GetPositionPtr());

	objectRender_ = std::make_unique<ObjectRender>();
	objectRender_->Initialize(drawDataManager_, modelManager_);
}

std::unique_ptr<IScene> ShigeScene::Update() {
	camera_->Update();
	Vector3 cameraPos = camera_->GetCenter();
	grid_->Update(cameraPos, camera_->GetVPMatrix());
	auto key = commonData_->keyManager->GetKeyStates();

	float deltaTime = engine_->GetFPSObserver()->GetDeltatime();

	worldTimer_ += deltaTime;
	if(worldTimer_ > 2.0f) {
		worldTimer_ = 0.0f;
		Vector3 initPos = { float(rand() % 20 - 10), 0.0f, float(rand() % 20 - 10) };
		enemyManager_->PopEnemy(initPos);
	}

	player_->Update(deltaTime);
	enemyManager_->Update(deltaTime);

	colliderManager_->CollisionCheckAll();

	//DrawInfoを収集して描画クラスに渡す
	drawInfos_.clear();
	drawInfos_.push_back(player_->GetDrawInfo());
	auto enemyDI = enemyManager_->GetEnemyDrawInfos();
	drawInfos_.insert(drawInfos_.end(), enemyDI.begin(), enemyDI.end());

	objectRender_->SetDrawInfo(drawInfos_.data(), drawInfos_.size(), camera_->GetVPMatrix());

	if (key[Key::Debug1]) {
		return std::make_unique<ShopScene>();
	}

	return nullptr;
}

void ShigeScene::Draw() {
	auto window = commonData_->mainWindow.second.get();
	auto display = commonData_->display.get();
	auto cmdObj = commonData_->cmdObject.get();

	display->PreDraw(cmdObj, true);

	grid_->Draw(cmdObj);
	objectRender_->Draw(cmdObj);

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
