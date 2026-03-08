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

	pieceManager_ = std::make_unique<PieceManager>();
	pieceManager_->Initialize(commonData_->pieces);
	//PieceManager内でstd::moveを行っているため、クリアを行う
	commonData_->pieces.clear();
	Piece::SetPieceManager(pieceManager_.get());

	shop_ = std::make_unique<Shop>();
	shop_->Initialize(itemManager_.get());

	pieceManager_->RefreshShopPieces(shop_->RefreshShopPieces());

	shopCursor_ = std::make_unique<ShopCursor>();
	shopCursor_->Initialize(commonData_->keyManager.get(), pieceManager_.get());

	weaponManager_ = std::make_unique<WeaponManager>();
	weaponManager_->InitializeData(modelManager_, drawDataManager_);

	weaponDebugger_ = std::make_unique<WeaponDebugger>(weaponManager_.get());

	parameterRender_ = std::make_unique<ParameterRender>();
	parameterRender_->Initialize(modelManager_, drawDataManager_, engine_);

	orthoCamera_ = std::make_unique<Camera>();
}

std::unique_ptr<IScene> ShopScene::Update() {
	auto key = commonData_->keyManager->GetKeyStates();

#ifdef USE_IMGUI

	itemManager_->DrawImGui();
	pieceManager_->UpdateItemInfo(itemManager_.get());
	shop_->Initialize(itemManager_.get());

#endif

	debugCamera_->Update();
	grid_->Update(debugCamera_->GetCenter(), debugCamera_->GetVPMatrix());

	//何かしらのトリガーでショップのピースを更新する
	if (key[Key::Debug2]) {
		pieceManager_->RefreshShopPieces(shop_->RefreshShopPieces());
	}

	shopCursor_->Update(debugCamera_.get());
	shopCursor_->EditPiece(backPack_.get());

	colliderManager_->CollisionCheckAll();

	//DrawInfo集め
	std::vector<DrawInfo> drawInfos = backPack_->GetSlotDrawInfos();

	auto pieces = pieceManager_->GetAllPieces();
	for(const auto& piece : pieces) {
		auto pieceDrawInfos = piece->GetDrawInfos();
		drawInfos.insert(drawInfos.end(), pieceDrawInfos.begin(), pieceDrawInfos.end());
	}
	objectRender_->SetDrawInfo(drawInfos.data(), drawInfos.size(), debugCamera_->GetVPMatrix());

	OrthographicDesc orthDesc;
	orthDesc.SetValue();
	orthoCamera_->SetProjectionMatrix(orthDesc);
	orthoCamera_->SetScale({1,-1,1});
	orthoCamera_->SetPosition({0, 0, 0});
	orthoCamera_->MakeMatrix();
	parameterRender_->Update(orthoCamera_->GetVPMatrix(), commonData_->playerParameterData);

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
	//parameterRender_->Draw(cmdObj);

	display->PostDraw(cmdObj);

	window->PreDraw(cmdObj, true);

#ifdef USE_IMGUI
	display->DrawImGui();
	weaponDebugger_->Draw();

	engine_->DrawImGui();
#endif

	window->PostDraw(cmdObj);
}
