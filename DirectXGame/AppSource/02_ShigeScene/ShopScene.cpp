#include "ShopScene.h"

ShopScene::~ShopScene() {
}

void ShopScene::Initialize() {
	colliderManager_ = std::make_unique<ColliderManager>();
	Collider::SetColliderManager(colliderManager_.get());

	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize(input_);
	
	debugCamera_->SetCenter(cameraCenter_);
	debugCamera_->SetSpherical(cameraSpherical_);

	PerspectiveFovDesc desc;
	desc.SetValue(cameraPerspectiveSize_.x, cameraPerspectiveSize_.y);
	debugCamera_->SetProjectionMatrix(desc);

	grid_ = std::make_unique<Grid>();
	grid_->Initialize(drawDataManager_);

	itemManager_ = std::make_unique<ItemManager>();
	itemManager_->Initialize(modelManager_);

	objectRender_ = std::make_unique<ObjectRender>();
	objectRender_->Initialize(drawDataManager_, modelManager_);
	
	// piece.objモデルを読み込み
	pieceModelID_ = modelManager_->LoadModel("Assets/Model/Piece");

	backPack_ = std::make_unique<BackPack>();
	backPack_->Initialize();
	backPack_->SetPieceModelID(pieceModelID_);

	pieceManager_ = std::make_unique<PieceManager>();
	pieceManager_->Initialize();
	//PieceManager内でstd::moveを行っているため、クリアを行う
	commonData_->pieces.clear();
	Piece::SetPieceManager(pieceManager_.get());
	Piece::pieceModelID = pieceModelID_;

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
	orthoCamera_->SetProjectionMatrix(OrthographicDesc{});

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

	// リロールバーの初期化
	InitializeRerollBar();

	displayRange_.top = 180.0f;
	displayRange_.bottom = 810.0f;
	displayRange_.right = 570.0f;
	displayRange_.left = 210.0f;
}

std::unique_ptr<IScene> ShopScene::Update() {
	auto key = commonData_->keyManager->GetKeyStates();

#ifdef USE_IMGUI

	ImGui::Begin("ShopScene Camera Settings");
	if (ImGui::DragFloat3("Camera Center", &cameraCenter_.x, 0.1f)) {
		debugCamera_->SetCenter(cameraCenter_);
	}
	if (ImGui::DragFloat3("Camera Spherical", &cameraSpherical_.x, 0.01f)) {
		debugCamera_->SetSpherical(cameraSpherical_);
	}
	if (ImGui::DragFloat2("Perspective Size", &cameraPerspectiveSize_.x, 1.0f)) {
		PerspectiveFovDesc desc;
		desc.SetValue(cameraPerspectiveSize_.x, cameraPerspectiveSize_.y);
		debugCamera_->SetProjectionMatrix(desc);
	}
	ImGui::Separator();
	ImGui::Text("Reroll Settings");
	ImGui::DragFloat("Reroll Interval Time", &rerollIntervalTime_, 0.1f, 0.0f, 10.0f);
	ImGui::Text("Interval Timer: %.2f / %.2f", rerollIntervalTimer_, rerollIntervalTime_);
	ImGui::Text("Reroll Count: %d", rerollCount_);
	ImGui::End();

	itemManager_->DrawImGui();
	pieceManager_->UpdateItemInfo(itemManager_.get());
	shop_->Initialize(itemManager_.get());
	shop_->DrawImGui();

#endif

	shopDisplay_->Update();

	debugCamera_->Update(false);
	grid_->Update(debugCamera_->GetCenter(), debugCamera_->GetVPMatrix());

	// 時間経過でリロールカウントを増やす
	if (useAutoReroll_) {
		shopRerollTimer_ += deltaTime_;
		if (shopRerollTimer_ >= shopRerollTime_) {
			rerollCount_++;
			shopRerollTimer_ -= shopRerollTime_; // 超過分を維持してタイマーリセット
		}
	} else {
		shopRerollTimer_ = 0.0f;
	}

	// ショップのアイテムがバックパックに配置されたことを検知
	if (pieceManager_->GetShopPieceCount() < 3) {
		if (!pendingReroll_) {
			pendingReroll_ = true;
			rerollIntervalTimer_ = 0.0f; // インターバルタイマーをリセット
		}
	}

	// リロール待機状態の場合、インターバルタイマーを進める
	if (pendingReroll_) {
		rerollIntervalTimer_ += deltaTime_;
	}

	// リロール待機状態で、インターバル経過、カウントが1以上、かつピースを持っていなければ更新実行
	if (pendingReroll_ && rerollCount_ > 0 && !shopCursor_->HasHeldPiece()) {
		pieceManager_->RefreshShopPieces(shop_->RefreshShopPieces());
		rerollCount_--;
		pendingReroll_ = false;
		rerollIntervalTimer_ = 0.0f;
	}

	// 何かしらのトリガーでショップのピースを更新する
	if (key[Key::Debug2]) {
		if (!shopCursor_->HasHeldPiece()) {
			pieceManager_->RefreshShopPieces(shop_->RefreshShopPieces());
			pendingReroll_ = false; // 強制更新されたので待機状態を解除
			rerollIntervalTimer_ = 0.0f; // インターバルタイマーもリセット
		}
	}

	shopCursor_->Update(debugCamera_.get());
	shopCursor_->EditPiece(backPack_.get());

	colliderManager_->CollisionCheckAll();

	//DrawInfo集め
	{
		std::vector<DrawInfo> drawInfos = backPack_->GetSlotDrawInfos();

		auto pieces = pieceManager_->GetAllPieces();
		drawInfos.reserve(drawInfos.size() + pieces.size());
		for (const auto& piece : pieces) {
			auto pieceDrawInfos = piece->GetDrawInfos();
			drawInfos.insert(drawInfos.end(), pieceDrawInfos.begin(), pieceDrawInfos.end());
		}
		objectRender_->SetDrawInfo(drawInfos.data(), drawInfos.size(), debugCamera_->GetVPMatrix());
	}

	{
		orthoCamera_->SetScale({ 1,-1,1 });
		orthoCamera_->SetPosition({ 0, 0, 0 });
		orthoCamera_->MakeMatrix();
	}
	//parameterRender_->Update(orthoCamera_->GetVPMatrix(), commonData_->playerParameterData);

	commonData_->pieces = pieceManager_->Update(backPack_.get(), deltaTime_);

	// リロールバーの更新
	UpdateRerollBar(orthoCamera_->GetVPMatrix());

	Matrix4x4 wvp = Matrix::MakeAffineMatrix(debugTransform_.scale, debugTransform_.rotate, debugTransform_.position) * debugCamera_->GetVPMatrix();
	debugObj_->CopyBufferData(0, &wvp, sizeof(wvp));
	debugObj_->CopyBufferData(1, &debugColor_, sizeof(debugColor_));

	return nullptr;
}

void ShopScene::DrawReady() {
	auto cmdObj = commonData_->cmdObject.get();

	shopDisplay_->PreDraw();

	//grid_->Draw(cmdObj);
	objectRender_->Draw(cmdObj);
	weaponDebugger_->Draw();
	//parameterRender_->Draw(cmdObj);
	//debugObj_->Draw(cmdObj);

	// リロールバーの描画
	DrawRerollBar(cmdObj);

	shopDisplay_->PostDraw();
}

void ShopScene::Draw() {
	shopDisplay_->Draw();
}

void ShopScene::InitializeRerollBar() {
	// リロールバーの生成
	rerollBarFill_.render = std::make_unique<SHEngine::RenderObject>("RerollBarFill");
	rerollBarBG_.render = std::make_unique<SHEngine::RenderObject>("RerollBarBG");

	// DrawDataの取得（Planeモデルを使用）
	auto drawData = drawDataManager_->GetDrawData(modelManager_->GetNodeModelData(0).drawDataIndex);

	// 前面バーの初期化
	rerollBarFill_.render->Initialize();
	rerollBarFill_.render->SetDrawData(drawData);
	rerollBarFill_.render->psoConfig_.vs = "Simple.VS.hlsl";
	rerollBarFill_.render->psoConfig_.ps = "Color.PS.hlsl";
	rerollBarFill_.render->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER, "WVP");
	rerollBarFill_.render->CreateCBV(sizeof(Vector4), ShaderType::PIXEL_SHADER, "Color");

	// 背景バーの初期化
	rerollBarBG_.render->Initialize();
	rerollBarBG_.render->SetDrawData(drawData);
	rerollBarBG_.render->psoConfig_.vs = "Simple.VS.hlsl";
	rerollBarBG_.render->psoConfig_.ps = "Color.PS.hlsl";
	rerollBarBG_.render->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER, "WVP");
	rerollBarBG_.render->CreateCBV(sizeof(Vector4), ShaderType::PIXEL_SHADER, "Color");

	// 単位行列の代入
	rerollBarFill_.wvp = Matrix4x4::Identity();
	rerollBarBG_.wvp = Matrix4x4::Identity();

	// 前面バー
	rerollBarFill_.transform.scale = { rerollBarSize_.x, rerollBarSize_.y, 1.0f };
	rerollBarFill_.transform.rotate = { 0.0f, 0.0f, 0.0f };
	rerollBarFill_.transform.position = { rerollBarPos_.x, rerollBarPos_.y, 0.0f };

	// 背景バー
	rerollBarBG_.transform.scale = { rerollBarSize_.x, rerollBarSize_.y, 1.0f };
	rerollBarBG_.transform.rotate = { 0.0f, 0.0f, 0.0f };
	rerollBarBG_.transform.position = { rerollBarPos_.x, rerollBarPos_.y, 0.0f };

	// テキストの初期化
	int planeModelHandle = modelManager_->LoadModel("Assets/.EngineResource/Model/Plane");
	auto planeModelData = modelManager_->GetNodeModelData(planeModelHandle);
	SHEngine::DrawData textDrawData = drawDataManager_->GetDrawData(planeModelData.drawDataIndex);

	rerollText_ = std::make_unique<SHEngine::Text>();
	rerollText_->Initialize(textDrawData, "YDWbananaslipplus.otf", 64);
	rerollText_->SetText(L"リロール");

	// 操作説明テキストの初期化
	controlText_ = std::make_unique<SHEngine::Text>();
	controlText_->Initialize(textDrawData, "YDWbananaslipplus.otf", 64);
	controlText_->SetText(L"持つ 離す");

	// ラクラク配置テキストの初期化
	easyPlaceText_ = std::make_unique<SHEngine::Text>();
	easyPlaceText_->Initialize(textDrawData, "YDWbananaslipplus.otf", 64);
	easyPlaceText_->SetText(L"自動配置");

	// 武器安置所テキストの初期化
	weaponStorageText_ = std::make_unique<SHEngine::Text>();
	weaponStorageText_->Initialize(textDrawData, "YDWbananaslipplus.otf", 64);
	weaponStorageText_->SetText(L"武器安置所");

	// マウスボタンスプライトの初期化
	mouseLeftTextureIndex_ = textureManager_->LoadTexture("Assets/Texture/UI/mouse_left.png");
	mouseRightTextureIndex_ = textureManager_->LoadTexture("Assets/Texture/UI/mouse_right.png");
	mouseLeftActiveTextureIndex_ = textureManager_->LoadTexture("Assets/Texture/UI/mouse_left_active.png");
	mouseRightActiveTextureIndex_ = textureManager_->LoadTexture("Assets/Texture/UI/mouse_right_active.png");

	mouseLeftSprite_ = std::make_unique<SHEngine::RenderObject>("MouseLeftSprite");
	mouseLeftSprite_->Initialize();
	mouseLeftSprite_->SetDrawData(drawData);
	mouseLeftSprite_->psoConfig_.vs = "Simple.VS.hlsl";
	mouseLeftSprite_->psoConfig_.ps = "PostEffect/Simple.PS.hlsl";
	mouseLeftSprite_->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER, "WVP");
	mouseLeftSprite_->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "TextureIndex");
	mouseLeftSprite_->SetUseTexture(true);
	mouseLeftSprite_->psoConfig_.depthStencilID = SHEngine::PSO::DepthStencilID::Transparent;

	mouseRightSprite_ = std::make_unique<SHEngine::RenderObject>("MouseRightSprite");
	mouseRightSprite_->Initialize();
	mouseRightSprite_->SetDrawData(drawData);
	mouseRightSprite_->psoConfig_.vs = "Simple.VS.hlsl";
	mouseRightSprite_->psoConfig_.ps = "PostEffect/Simple.PS.hlsl";
	mouseRightSprite_->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER, "WVP");
	mouseRightSprite_->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "TextureIndex");
	mouseRightSprite_->SetUseTexture(true);
	mouseRightSprite_->psoConfig_.depthStencilID = SHEngine::PSO::DepthStencilID::Transparent;
}

void ShopScene::UpdateRerollBar(Matrix4x4 vpMatrix) {
	// リロール進行率の計算
	float progress = shopRerollTimer_ / shopRerollTime_;
	if (progress > 1.0f) progress = 1.0f;

	// 前面バーのスケールを進行率に応じて変更
	float currentScale = rerollBarSize_.x * progress;
	rerollBarFill_.transform.scale.x = currentScale;

	// バーの位置調整（左端から右に伸びるように）
	float offsetX = (rerollBarSize_.x - currentScale) / 2.0f;
	rerollBarFill_.transform.position.x = rerollBarPos_.x - offsetX;
	rerollBarFill_.transform.position.y = rerollBarPos_.y;

#ifdef USE_IMGUI
	// ImGuiでのサイズ・位置変更を即座に反映
	rerollBarBG_.transform.scale = { rerollBarSize_.x, rerollBarSize_.y, 1.0f };
	rerollBarBG_.transform.position = { rerollBarPos_.x, rerollBarPos_.y, 0.0f };
	rerollBarFill_.transform.scale.y = rerollBarSize_.y;
#endif

	// 前面バーの行列計算
	rerollBarFill_.wvp = Matrix::MakeAffineMatrix(rerollBarFill_.transform.scale, rerollBarFill_.transform.rotate, rerollBarFill_.transform.position);
	rerollBarFill_.wvp *= vpMatrix;
	rerollBarFill_.render->CopyBufferData(0, &rerollBarFill_.wvp, sizeof(Matrix4x4));

	Vector4 fillColor = { 0.0f, 0.8f, 1.0f, 1.0f }; // 青色
	rerollBarFill_.render->CopyBufferData(1, &fillColor, sizeof(Vector4));

	// 背景バーの行列計算
	rerollBarBG_.wvp = Matrix::MakeAffineMatrix(rerollBarBG_.transform.scale, rerollBarBG_.transform.rotate, rerollBarBG_.transform.position);
	rerollBarBG_.wvp *= vpMatrix;
	rerollBarBG_.render->CopyBufferData(0, &rerollBarBG_.wvp, sizeof(Matrix4x4));

	Vector4 bgColor = { 0.3f, 0.3f, 0.3f, 0.8f }; // 暗い灰色
	rerollBarBG_.render->CopyBufferData(1, &bgColor, sizeof(Vector4));

	// テキストの更新
	std::wstring text = L"リロール ( " + std::to_wstring(rerollCount_) + L" )";
	rerollText_->SetText(text.c_str());

	rerollText_->SetColor(rerollTextColor_);
	rerollText_->SetTransform(rerollTextTransform_);
	rerollText_->Update(vpMatrix);

	// 操作説明テキストの更新
	controlText_->SetColor(controlTextColor_);
	controlText_->SetTransform(controlTextTransform_);
	controlText_->Update(vpMatrix);

	// ラクラク配置テキストの更新
	// アイテムを掴んでいるかどうかで表示を変更
	if (shopCursor_->HasHeldPiece()) {
		easyPlaceText_->SetText(L"回転");
	} else {
		easyPlaceText_->SetText(L"自動配置");
	}
	easyPlaceText_->SetColor(easyPlaceTextColor_);
	easyPlaceText_->SetTransform(easyPlaceTextTransform_);
	easyPlaceText_->Update(vpMatrix);

	// 武器安置所テキストの更新
	weaponStorageText_->SetColor(weaponStorageTextColor_);
	weaponStorageText_->SetTransform(weaponStorageTextTransform_);
	weaponStorageText_->Update(vpMatrix);

	// マウスボタンスプライトの更新
	// マウスの状態を取得
	auto mouseButtons = input_->GetMouseButtonState();
	bool isRightPressed = mouseButtons && (mouseButtons[0] & 0x80);
	bool isLeftPressed = mouseButtons && (mouseButtons[1] & 0x80);

	// 左ボタンスプライトの更新
	Matrix4x4 mouseLeftWVP = Matrix::MakeAffineMatrix(mouseLeftTransform_.scale, mouseLeftTransform_.rotate, mouseLeftTransform_.position);
	mouseLeftWVP *= vpMatrix;
	mouseLeftSprite_->CopyBufferData(0, &mouseLeftWVP, sizeof(Matrix4x4));
	int leftTexIndex = isLeftPressed ? mouseLeftActiveTextureIndex_ : mouseLeftTextureIndex_;
	mouseLeftSprite_->CopyBufferData(1, &leftTexIndex, sizeof(int));

	// 右ボタンスプライトの更新
	Matrix4x4 mouseRightWVP = Matrix::MakeAffineMatrix(mouseRightTransform_.scale, mouseRightTransform_.rotate, mouseRightTransform_.position);
	mouseRightWVP *= vpMatrix;
	mouseRightSprite_->CopyBufferData(0, &mouseRightWVP, sizeof(Matrix4x4));
	int rightTexIndex = isRightPressed ? mouseRightActiveTextureIndex_ : mouseRightTextureIndex_;
	mouseRightSprite_->CopyBufferData(1, &rightTexIndex, sizeof(int));

#ifdef USE_IMGUI
	ImGui::Begin("Shop Reroll Bar");
	ImGui::Text("Reroll Text Settings");
	ImGui::DragFloat2("Reroll Text Position", &rerollTextTransform_.position.x, 1.0f);
	ImGui::DragFloat2("Reroll Text Size", &rerollTextTransform_.scale.x, 0.1f, 0.1f, 10.0f);
	ImGui::ColorEdit4("Reroll Text Color", &rerollTextColor_.x);
	ImGui::Separator();
	ImGui::Text("Control Text Settings");
	ImGui::DragFloat2("Control Text Position", &controlTextTransform_.position.x, 1.0f);
	ImGui::DragFloat3("Control Text Size", &controlTextTransform_.scale.x, 0.1f);
	ImGui::ColorEdit4("Control Text Color", &controlTextColor_.x);
	ImGui::Separator();
	ImGui::Text("Easy Place Text Settings");
	ImGui::DragFloat2("Easy Place Text Position", &easyPlaceTextTransform_.position.x, 1.0f);
	ImGui::DragFloat3("Easy Place Text Size", &easyPlaceTextTransform_.scale.x, 0.1f);
	ImGui::ColorEdit4("Easy Place Text Color", &easyPlaceTextColor_.x);
	ImGui::Separator();
	ImGui::Text("Weapon Storage Text Settings");
	ImGui::DragFloat2("Weapon Storage Text Position", &weaponStorageTextTransform_.position.x, 1.0f);
	ImGui::DragFloat3("Weapon Storage Text Size", &weaponStorageTextTransform_.scale.x, 0.1f);
	ImGui::ColorEdit4("Weapon Storage Text Color", &weaponStorageTextColor_.x);
	ImGui::Separator();
	ImGui::Text("Mouse Sprite Settings");
	ImGui::DragFloat3("Mouse Left Position", &mouseLeftTransform_.position.x, 1.0f);
	ImGui::DragFloat3("Mouse Left Scale", &mouseLeftTransform_.scale.x, 1.0f);
	ImGui::DragFloat3("Mouse Right Position", &mouseRightTransform_.position.x, 1.0f);
	ImGui::DragFloat3("Mouse Right Scale", &mouseRightTransform_.scale.x, 1.0f);
	ImGui::Separator();
	ImGui::Text("Reroll Timer: %.2f / %.2f", shopRerollTimer_, shopRerollTime_);
	ImGui::Text("Reroll Count: %d", rerollCount_);
	ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f));
	ImGui::End();
#endif
}

void ShopScene::DrawRerollBar(CmdObj* cmdObj) {
	// 背景から前面の順に描画
	rerollBarBG_.render->Draw(cmdObj);
	rerollBarFill_.render->Draw(cmdObj);

	// テキスト描画
	rerollText_->Draw(cmdObj);
	controlText_->Draw(cmdObj);
	easyPlaceText_->Draw(cmdObj);
	weaponStorageText_->Draw(cmdObj);

	// マウススプライト描画
	mouseLeftSprite_->Draw(cmdObj);
	mouseRightSprite_->Draw(cmdObj);
}
