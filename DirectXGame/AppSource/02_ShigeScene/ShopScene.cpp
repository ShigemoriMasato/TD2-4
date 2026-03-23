#include "ShopScene.h"

ShopScene::~ShopScene() {
}

void ShopScene::Initialize() {
	colliderManager_ = std::make_unique<ColliderManager>();
	Collider::SetColliderManager(colliderManager_.get());

	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize(input_);
	
	debugCamera_->SetCenter({ -5.0f, -20.0f, -4.5f });
	debugCamera_->SetSpherical({ 20.0f, 0.0f, -1.570f });

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
	pieceManager_->Initialize();
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

	//parameterRender_ = std::make_unique<ParameterRender>();
	//parameterRender_->Initialize(modelManager_, drawDataManager_, engine_);

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
	shop_->DrawImGui();

#endif

	shopDisplay_->Update();

	debugCamera_->Update(false);
	grid_->Update(debugCamera_->GetCenter(), debugCamera_->GetVPMatrix());

	// 時間経過で自動でショップの内容を更新する
	if (useAutoReroll_) {
		shopRerollTimer_ += deltaTime_;
		if (shopRerollTimer_ >= shopRerollTime_) {
			if (!shopCursor_->HasHeldPiece()) {
				pieceManager_->RefreshShopPieces(shop_->RefreshShopPieces());
				shopRerollTimer_ = 0.0f;
			}
		}
	} else {
		shopRerollTimer_ = 0.0f;
	}

	//何かしらのトリガーでショップのピースを更新する
	if (key[Key::Debug2]) {
		if (!shopCursor_->HasHeldPiece()) {
			pieceManager_->RefreshShopPieces(shop_->RefreshShopPieces());
			shopRerollTimer_ = 0.0f;
		}
	}

	shopCursor_->Update(debugCamera_.get());
	shopCursor_->EditPiece(backPack_.get());

	colliderManager_->CollisionCheckAll();

	//DrawInfo集め
	{
		std::vector<DrawInfo> drawInfos = backPack_->GetSlotDrawInfos();

		auto pieces = pieceManager_->GetAllPieces();
		for (const auto& piece : pieces) {
			auto pieceDrawInfos = piece->GetDrawInfos();
			drawInfos.insert(drawInfos.end(), pieceDrawInfos.begin(), pieceDrawInfos.end());
		}
		objectRender_->SetDrawInfo(drawInfos.data(), drawInfos.size(), debugCamera_->GetVPMatrix());
	}

	{
		OrthographicDesc orthDesc;
		orthDesc.SetValue();
		orthoCamera_->SetProjectionMatrix(orthDesc);
		orthoCamera_->SetScale({ 1,-1,1 });
		orthoCamera_->SetPosition({ 0, 0, 0 });
		orthoCamera_->MakeMatrix();
	}
	//parameterRender_->Update(orthoCamera_->GetVPMatrix(), commonData_->playerParameterData);

	commonData_->pieces = pieceManager_->Update(backPack_.get(), deltaTime_);

	Matrix4x4 wvp = Matrix::MakeAffineMatrix(debugTransform_.scale, debugTransform_.rotate, debugTransform_.position) * debugCamera_->GetVPMatrix();
	debugObj_->CopyBufferData(0, &wvp, sizeof(wvp));
	debugObj_->CopyBufferData(1, &debugColor_, sizeof(debugColor_));

	return nullptr;
}

void ShopScene::DrawReady() {
	auto cmdObj = commonData_->cmdObject.get();

	shopDisplay_->PreDraw();

	grid_->Draw(cmdObj);
	objectRender_->Draw(cmdObj);
	weaponDebugger_->Draw();
	//parameterRender_->Draw(cmdObj);
	//debugObj_->Draw(cmdObj);

	shopDisplay_->PostDraw();
}

void ShopScene::Draw() {
	shopDisplay_->Draw();
}
