#include "ShopScene.h"
#include "ShigeScene.h"

void ShopScene::Initialize() {
	colliderManager_ = std::make_unique<ColliderManager>();
	Collider::SetColliderManager(colliderManager_.get());
	
	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize(input_);

	grid_ = std::make_unique<Grid>();
	grid_->Initialize(drawDataManager_);

	itemManager_ = std::make_unique<ItemManager>();
	itemManager_->Initialize(modelManager_);

	objectRender_ = std::make_unique<ObjectRender>();
	objectRender_->Initialize(drawDataManager_, modelManager_);

	backPack_ = std::make_unique<BackPack>();
	backPack_->Initialize();

	pieces_.resize(1);
	for (auto& piece : pieces_) {
		piece = std::make_unique<Piece>();
		auto item = itemManager_->GetItem(0);
		piece->Initialize(item);
		piece->SetPosition({ 2.0f, 0.0f, 2.0f });
	}
}

std::unique_ptr<IScene> ShopScene::Update() {
	auto key = commonData_->keyManager->GetKeyStates();

	debugCamera_->Update();
	grid_->Update(debugCamera_->GetCenter(), debugCamera_->GetVPMatrix());

	colliderManager_->CollisionCheckAll();

	//DrawInfo集め
	std::vector<DrawInfo> drawInfos = backPack_->GetSlotDrawInfos();
	objectRender_->SetDrawInfo(drawInfos.data(), drawInfos.size(), debugCamera_->GetVPMatrix());

	if (key[Key::Debug1]) {
		return std::unique_ptr<ShigeScene>();
	}

	return nullptr;
}

void ShopScene::Draw() {
	// シーン遷移のテスト用に背景を青く塗る
	auto window = commonData_->mainWindow.second.get();
	auto display = commonData_->display.get();
	auto cmdObj = commonData_->cmdObject.get();
	display->PreDraw(cmdObj, true);

	grid_->Draw(cmdObj);
	objectRender_->Draw(cmdObj);

	display->PostDraw(cmdObj);

	window->PreDraw(cmdObj, true);

#ifdef USE_IMGUI
	display->DrawImGui();
	itemManager_->DrawImGui();

	engine_->DrawImGui();
#endif

	window->PostDraw(cmdObj);
}
