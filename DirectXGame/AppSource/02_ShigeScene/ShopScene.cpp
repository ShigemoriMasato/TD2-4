#include "ShopScene.h"
#include "ShigeScene.h"

ShopScene::~ShopScene() {
	commonData_->pieces = pieceManager_->GetHoldPieces();
}

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

	shopCursor_ = std::make_unique<ShopCursor>();
	shopCursor_->Initialize(commonData_->keyManager.get());

	pieceManager_ = std::make_unique<PieceManager>();
	pieceManager_->Initialize(commonData_->pieces);
	//PieceManager内でstd::moveを行っているため、クリアを行う
	commonData_->pieces.clear();
	Piece::SetPieceManager(pieceManager_.get());

	shop_ = std::make_unique<Shop>();
	shop_->Initialize(itemManager_.get());

	pieceManager_->RefreshShopPieces(shop_->RefreshShopPieces());

	weaponManager_ = std::make_unique<WeaponManager>();
	weaponManager_->InitializeData(modelManager_, drawDataManager_);

	weaponDebugger_ = std::make_unique<WeaponDebugger>(weaponManager_.get());
}

std::unique_ptr<IScene> ShopScene::Update() {
	auto key = commonData_->keyManager->GetKeyStates();

	debugCamera_->Update();
	grid_->Update(debugCamera_->GetCenter(), debugCamera_->GetVPMatrix());

	//何かしらのトリガーでショップのピースを更新する
	if (key[Key::Debug2]) {
		pieceManager_->RefreshShopPieces(shop_->RefreshShopPieces());
	}

	auto pieces = pieceManager_->GetAllPieces();

	shopCursor_->Update(debugCamera_.get());
	shopCursor_->EditPiece(pieces, backPack_.get());

	colliderManager_->CollisionCheckAll();

	//DrawInfo集め
	std::vector<DrawInfo> drawInfos = backPack_->GetSlotDrawInfos();
	for(const auto& piece : pieces) {
		auto pieceDrawInfos = piece->GetDrawInfos();
		drawInfos.insert(drawInfos.end(), pieceDrawInfos.begin(), pieceDrawInfos.end());
	}
	objectRender_->SetDrawInfo(drawInfos.data(), drawInfos.size(), debugCamera_->GetVPMatrix());

	if (key[Key::Debug1]) {
		return std::make_unique<ShigeScene>();
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
	weaponDebugger_->Draw();

	engine_->DrawImGui();
#endif

	window->PostDraw(cmdObj);
}
