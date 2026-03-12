#include "ShopScene.h"

ShopScene::~ShopScene() {
	commonData_->pieces = pieceManager_->GetHoldPieces();
}

void ShopScene::Initialize() {
	colliderManager_ = std::make_unique<ColliderManager>();
	Collider::SetColliderManager(colliderManager_.get());
	
	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize(input_);
	PerspectiveFovDesc desc;
	desc.SetValue(640.0f);
	debugCamera_->SetProjectionMatrix(desc);

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

	shopDisplay_ = std::make_unique<ShopDisplay>();
	//PlaneのDrawDataを取得
	auto drawData = drawDataManager_->GetDrawData(modelManager_->GetNodeModelData(1).drawDataIndex);
	shopDisplay_->Initialize(commonData_->cmdObject.get(), drawData, textureManager_);

	debugObj_ = std::make_unique<SHEngine::RenderObject>("ShopDebug");
	debugObj_->Initialize();
	debugObj_->SetDrawData(drawDataManager_->GetDrawData(modelManager_->GetNodeModelData(0).drawDataIndex));
	debugObj_->psoConfig_.vs = "Simple.VS.hlsl";
	debugObj_->psoConfig_.ps = "Color.PS.hlsl";
	debugObj_->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER, "WVP");
	debugObj_->CreateCBV(sizeof(Vector4), ShaderType::PIXEL_SHADER, "Color");
}

std::unique_ptr<IScene> ShopScene::Update() {
	auto key = commonData_->keyManager->GetKeyStates();

#ifdef USE_IMGUI

	itemManager_->DrawImGui();
	pieceManager_->UpdateItemInfo(itemManager_.get());
	shop_->Initialize(itemManager_.get());

#endif

	shopDisplay_->Update();

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
	//parameterRender_->Update(orthoCamera_->GetVPMatrix(), commonData_->playerParameterData);

	Matrix4x4 wvp = Matrix::MakeAffineMatrix(debugTransform_.scale, debugTransform_.rotate, debugTransform_.position) * debugCamera_->GetVPMatrix();
	debugObj_->CopyBufferData(0, &wvp, sizeof(wvp));
	debugObj_->CopyBufferData(1, &debugColor_, sizeof(debugColor_));

#ifdef USE_IMGUI

	ImGui::Begin("DebugObj");

	ImGui::DragFloat3("Scale", &debugTransform_.scale.x, 0.1f);
	ImGui::DragFloat3("Rotate", &debugTransform_.rotate.x, 0.1f);
	ImGui::DragFloat3("Position", &debugTransform_.position.x, 0.1f);
	ImGui::ColorEdit4("Color", &debugColor_.x);

	ImGui::End();

#endif

	return nullptr;
}

void ShopScene::DrawReady() {
	auto cmdObj = commonData_->cmdObject.get();

	shopDisplay_->PreDraw();

	grid_->Draw(cmdObj);
	objectRender_->Draw(cmdObj);
	//parameterRender_->Draw(cmdObj);
	debugObj_->Draw(cmdObj);

	shopDisplay_->PostDraw();
}

void ShopScene::Draw() {
	shopDisplay_->Draw();
}
