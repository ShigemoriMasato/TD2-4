#include "TitleScene.h"
#include <imgui/imgui.h>
#include <Utility/Color.h>

#include <02_ShigeScene/ShigeScene.h>

#include <../Engine/Assets/Audio/AudioManager.h>
#include <Common/KeyConfig/WorldCursor.h>
#include <Utility/Matrix.h>
#include <Utility/MatrixFactory.h>
#include <numbers>

TitleScene::TitleScene() {
}

TitleScene::~TitleScene() {
	bgm_->Stop();
}

void TitleScene::Initialize() {
	// CommonDataの音量設定へのポインタを取得
	masterVolume_ = &commonData_->masterVolume;
	bgmVolume_ = &commonData_->bgmVolume;
	seVolume_ = &commonData_->seVolume;

	// 音量を計算
	UpdateCalculatedVolumes();

	titleUI_ = std::make_unique<TitleUI>();
	titleUI_->Initialize(drawDataManager_, modelManager_, commonData_);

	camera_ = std::make_unique<Camera>();
	PerspectiveFovDesc perspectiveDesc{};
	camera_->SetProjectionMatrix(perspectiveDesc);
	camera_->SetPosition({ 0.0f, 0.0f, -10.0f });
	camera_->SetRotation({ -0.5f, 0.0f, 0.0f });
	camera_->SetScale({ 1.0f, 1.0f, 1.0f });
	camera_->MakeMatrix();

	orthoCamera_ = std::make_unique<Camera>();

	gameCamera_ = std::make_unique<GameCamera>();
	gameCamera_->Initialize();
	gameCamera_->SetInput(input_);
	gameCamera_->SetOffset({ 0.0f, 35.0f, -35.0f });
	gameCamera_->Setrotation({ -0.75f, 0.0f, 0.0f });

	mapInfo_ = {
		.minX = -25.0f,
		.maxX = 25.0f,
		.minZ = -25.0f,
		.maxZ = 25.0f,
		.centerX = 15.0f,
		.centerZ = -10.0f,	
		.radius = 25.0f
	};

	map_ = std::make_unique<Map>();
	map_->Initialize(drawDataManager_, modelManager_);
	map_->SetStageTransform({ 0,-160,250 }, {0,0,0}, { 5,5,5 });
	map_->EnableStageAutoRotation(false); // 自動回転を無効化
	map_->SetMapInfo(mapInfo_);

	shopScene_ = std::make_unique<ShopScene>();
	shopScene_->Ready(engine_, commonData_);
	shopScene_->Initialize();

	player_ = std::make_unique<Player::Base>();
	player_->Initialize(modelManager_, drawDataManager_, CharacterID::Warrior, shopScene_->GetItemManager());
	// TitleSceneではPlayerの位置を調整（Mapと一緒に表示）
	player_->SetPosition({15.0f, 0.0f, -10.0f});
	player_->SetRotate({0.0f, 0.0f, 0.0f});

	player_->SetMapInfo(map_->GetMapInfo());

	// コントローラーの設定（InputControllerを使用）
	inputController_ = std::make_unique<InputController>(input_);
	inputController_->SetCurrentPosition(player_->GetPositionPtr());
	player_->SetController(inputController_.get());

	// ターゲットマーカーの初期化
	targetMarkerRender_ = std::make_unique<SHEngine::RenderObject>("TargetMarker");
	targetMarkerRender_->Initialize();
	targetMarkerRender_->psoConfig_.vs = "Game/Field.VS.hlsl";
	targetMarkerRender_->psoConfig_.ps = "Game/Field.PS.hlsl";
	targetMarkerRender_->SetUseTexture(true);

	int maruHandle = modelManager_->LoadModel("Assets/Model/Maru");
	auto maruModelData = modelManager_->GetNodeModelData(maruHandle);
	auto maruDrawData = drawDataManager_->GetDrawData(maruModelData.drawDataIndex);
	targetMarkerRender_->SetDrawData(maruDrawData);

	targetMarkerRender_->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER);
	targetMarkerRender_->CreateCBV(sizeof(Vector4), ShaderType::PIXEL_SHADER, "Color");
	targetMarkerRender_->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "TextureIndex");
	targetMarkerRender_->CreateCBV(sizeof(DirectionalLight), ShaderType::PIXEL_SHADER, "DirectionalLight");

	targetMarkerTexIndex_ = maruModelData.materials[maruModelData.materialIndex.front()].textureIndex;
	Vector4 markerColor = {1.0f, 1.0f, 1.0f, 1.0f};
	targetMarkerRender_->CopyBufferData(1, &markerColor, sizeof(markerColor));
	targetMarkerRender_->CopyBufferData(2, &targetMarkerTexIndex_, sizeof(targetMarkerTexIndex_));

	targetMarkerTransform_.scale = {1.0f, 1.0f, 1.0f};

	// yuka.objの初期化
	yukaRender_ = std::make_unique<SHEngine::RenderObject>("Yuka");
	yukaRender_->Initialize();
	yukaRender_->psoConfig_.vs = "Game/Field.VS.hlsl";
	yukaRender_->psoConfig_.ps = "Game/Field.PS.hlsl";
	yukaRender_->SetUseTexture(true);

	int yukaHandle = modelManager_->LoadModel("Assets/Model/yuka");
	auto yukaModelData = modelManager_->GetNodeModelData(yukaHandle);
	auto yukaDrawData = drawDataManager_->GetDrawData(yukaModelData.drawDataIndex);
	yukaRender_->SetDrawData(yukaDrawData);

	yukaRender_->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER);
	yukaRender_->CreateCBV(sizeof(Vector4), ShaderType::PIXEL_SHADER, "Color");
	yukaRender_->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "TextureIndex");
	yukaRender_->CreateCBV(sizeof(DirectionalLight), ShaderType::PIXEL_SHADER, "DirectionalLight");

	yukaTexIndex_ = yukaModelData.materials[yukaModelData.materialIndex.front()].textureIndex;
	Vector4 yukaColor = {1.0f, 0.0f, 0.0f, 1.0f};
	yukaRender_->CopyBufferData(1, &yukaColor, sizeof(yukaColor));
	yukaRender_->CopyBufferData(2, &yukaTexIndex_, sizeof(yukaTexIndex_));

	yukaTransform_.position = {15.0f, 0.0f, -10.0f};
	yukaTransform_.rotate = {0.0f, 0.0f, 0.0f};
	yukaTransform_.scale = {1.0f, 1.0f, 1.0f};

	// ライトの設定
	dirLight_.color = {1.0f, 1.0f, 1.0f, 1.0f};
	dirLight_.direction = {0.0f, 1.0f, 0.0f};
	dirLight_.intensity = 1.0f;

	bgm_ = AudioManager::GetInstance()->GetData("TitleScene.mp3")->CustomPlay(255);

	postEffect_ = std::make_unique<PostEffect>();
	postEffect_->Initialize(textureManager_, drawDataManager_->GetDrawData(commonData_->postEffectDrawDataIndex), true);
	postEffectConfig_.cmdObj = commonData_->cmdObject.get();
	postEffectConfig_.origin = commonData_->display->GetDisplay();

	fadeManager_ = std::make_unique<FadeManager>();
	fadeManager_->Initialize(modelManager_, drawDataManager_);
	fadeManager_->StartFadeOut(false);

	// グリッドの初期化
	grid_ = std::make_unique<Grid>();
	grid_->Initialize(drawDataManager_);

	SHEngine::DrawData planeDrawData = drawDataManager_->GetDrawData(modelManager_->GetNodeModelData(1).drawDataIndex);
	gameFrame_ = std::make_unique<GameFrame>();
	gameFrame_->Initialize(planeDrawData, textureManager_->LoadTexture("TitleFrame.png"));

	// マウスカーソルスプライトの初期化
	mouseCursorTexDefault_ = textureManager_->LoadTexture("Assets/Texture/UI/mouse.png");
	mouseCursorTexLeft_    = textureManager_->LoadTexture("Assets/Texture/UI/mouseL.png");
	mouseCursorTexRight_   = textureManager_->LoadTexture("Assets/Texture/UI/mouseR.png");
	mouseCursorTexBoth_    = textureManager_->LoadTexture("Assets/Texture/UI/mouseD.png");
	mouseCursorTextureIndex_ = mouseCursorTexDefault_;
	mouseCursorSprite_ = std::make_unique<SHEngine::RenderObject>("MouseCursorSprite");
	mouseCursorSprite_->Initialize();
	mouseCursorSprite_->SetDrawData(planeDrawData);
	mouseCursorSprite_->psoConfig_.vs = "Simple.VS.hlsl";
	mouseCursorSprite_->psoConfig_.ps = "PostEffect/Simple.PS.hlsl";
	mouseCursorSprite_->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER, "WVP");
	mouseCursorSprite_->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "TextureIndex");
	mouseCursorSprite_->SetUseTexture(true);
	mouseCursorSprite_->psoConfig_.depthStencilID = SHEngine::PSO::DepthStencilID::Transparent;
}

std::unique_ptr<IScene> TitleScene::Update() {
	auto keys = commonData_->keyManager->GetKeyStates();

	float deltaTime = engine_->GetFPSObserver()->GetDeltatime();

	shopScene_->SetDeltaTime(deltaTime);
	shopScene_->Update();

	// GameCameraをPlayerの位置に追従させる
	Vector3 cameraTargetPos = player_->GetTransform().position;
	gameCamera_->Update(deltaTime, cameraTargetPos);

	// Playerの位置に基づいてUI選択を更新
	{
		Vector3 playerPos = player_->GetTransform().position;
		bool inRange = false;
		bool inFrame1 = false;
		bool inFrame3 = false;
		if (std::abs(playerPos.x - 8.0f) <= 4.5f && std::abs(playerPos.z - 0.0f) <= 2.0f) {
			titleUI_->SetCurrentSelect(Title::Select::Start);
			inRange = true;
			inFrame1 = true;
		} else if (std::abs(playerPos.x - (-200.0f)) <= 4.5f && std::abs(playerPos.z - (-10.0f)) <= 2.0f) {
			titleUI_->SetCurrentSelect(Title::Select::Option);
			inRange = true;
		} else if (std::abs(playerPos.x - 22.0f) <= 4.5f && std::abs(playerPos.z - 0.0f) <= 2.0f) {
			titleUI_->SetCurrentSelect(Title::Select::Quit);
			inRange = true;
			inFrame3 = true;
		}
		titleUI_->SetPlayerInRange(inRange);

		// Frame1滞在タイマー
		if (inFrame1) {
			frame1StayTimer_ += deltaTime;
			if (frame1StayTimer_ >= kFrame1StayDuration_) {
				fadeManager_->StartFadeIn([]() { return std::make_unique<ShigeScene>(); });
			}
		} else {
			frame1StayTimer_ = 0.0f;
		}

		// Frame3滞在タイマー
		if (inFrame3) {
			frame3StayTimer_ += deltaTime;
			if (frame3StayTimer_ >= kFrame1StayDuration_) {
				commonData_->shouldQuit = true;
			}
		} else {
			frame3StayTimer_ = 0.0f;
		}
	}

	titleUI_->SetPlayer(player_.get());

	titleUI_->Update(gameCamera_->GetVPMatrix(), deltaTime);
	titleUI_->UpdateSelection(keys[Key::Tr_Up], keys[Key::Tr_Down]);

	// クリックでPlayerを移動させる処理
	if (keys[Key::Target]) {
		// マウスのカーソル座標を取得
		Vector2 cursorPos = commonData_->keyManager->GetCursorPos();

		// ワールド座標に変換
		Vector3 clickWorldPos = GetWorldCursor(gameCamera_.get(), cursorPos);

		// マップ円形範囲内に制限した座標を取得（Playerの移動範囲に合わせる）
		Vector3 clampedPos = map_->ClampToCircularBounds(clickWorldPos);
		// Playerを指定のワールド座標へ移動させる
		player_->GetController()->SetTargetPosition(clampedPos);

		// Compassの退場アニメーションを開始
		titleUI_->StartCompassExitAnimation();
	}

	map_->Update(gameCamera_->GetVPMatrix(), deltaTime);

	// Playerの更新を追加
	player_->SetDirectionalLight(dirLight_);
	player_->Update(gameCamera_->GetVPMatrix(), deltaTime, keys);

	// ターゲットマーカーの更新
	if (player_->GetController()->HasTarget()) {
		isTargetMarkerVisible_ = true;
		targetMarkerTransform_.position = player_->GetController()->GetTargetPosition();
		// Zファイティング（地面とのチラつき）を防ぐために少しY座標を浮かせる
		targetMarkerTransform_.position.y += 0.02f;

		targetMarkerAnimTimer_ += deltaTime;
		// XZに拡縮 (点滅のように見えないよう、速度を少し緊やかに調整)
		float scale = 1.0f + 0.2f * std::sin(targetMarkerAnimTimer_ * 5.0f);
		targetMarkerTransform_.scale = {scale, 1.0f, scale}; // Y軸方向は拡縮させない

		Matrix4x4 wvp = Matrix::MakeScaleMatrix(targetMarkerTransform_.scale) * Matrix::MakeRotationMatrix(targetMarkerTransform_.rotate) *
						Matrix::MakeTranslationMatrix(targetMarkerTransform_.position) * gameCamera_->GetVPMatrix();

		Vector4 markerColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		targetMarkerRender_->CopyBufferData(0, &wvp, sizeof(wvp));
		targetMarkerRender_->CopyBufferData(1, &markerColor, sizeof(markerColor));
		targetMarkerRender_->CopyBufferData(2, &targetMarkerTexIndex_, sizeof(targetMarkerTexIndex_));
		targetMarkerRender_->CopyBufferData(3, &dirLight_, sizeof(DirectionalLight));
	} else {
		isTargetMarkerVisible_ = false;
	}

	{
		// yukaの更新
		Matrix4x4 yukaWvp = Matrix::MakeScaleMatrix(yukaTransform_.scale) * Matrix::MakeRotationMatrix(yukaTransform_.rotate) *
			Matrix::MakeTranslationMatrix(yukaTransform_.position) * gameCamera_->GetVPMatrix();

		Vector4 yukaColor = { 1.0f, 0.0f, 0.0f, 1.0f };
		yukaRender_->CopyBufferData(0, &yukaWvp, sizeof(yukaWvp));
		yukaRender_->CopyBufferData(1, &yukaColor, sizeof(yukaColor));
		yukaRender_->CopyBufferData(2, &yukaTexIndex_, sizeof(yukaTexIndex_));
		yukaRender_->CopyBufferData(3, &dirLight_, sizeof(DirectionalLight));
	}

	// グリッドの更新
	grid_->Update({ 0.0f, 0.0f, 0.0f }, gameCamera_->GetVPMatrix());

	fadeManager_->Update(camera_->GetVPMatrix(), deltaTime);

	gameFrame_->Update();

	// orthoCameraの設定
	OrthographicDesc orthDesc;
	orthDesc.SetValue();
	orthoCamera_->SetProjectionMatrix(orthDesc);
	orthoCamera_->SetScale({1, -1, 1});
	orthoCamera_->SetPosition({0, 0, 0});
	orthoCamera_->MakeMatrix();

	// マウスカーソルスプライトの更新
	{
		bool leftClick  = (input_->GetMouseButtonState()[0] & 0x80) != 0;
		bool rightClick = (input_->GetMouseButtonState()[1] & 0x80) != 0;
		if (leftClick && rightClick) {
			mouseCursorTextureIndex_ = mouseCursorTexBoth_;
		} else if (leftClick) {
			mouseCursorTextureIndex_ = mouseCursorTexLeft_;
		} else if (rightClick) {
			mouseCursorTextureIndex_ = mouseCursorTexRight_;
		} else {
			mouseCursorTextureIndex_ = mouseCursorTexDefault_;
		}
		Vector2 cursorPos = input_->GetCursorPos();
		mouseCursorTransform_.position = {cursorPos.x, cursorPos.y * -1.0f, 0.0f};
		Matrix4x4 wvp = Matrix::MakeAffineMatrix(mouseCursorTransform_.scale, mouseCursorTransform_.rotate, mouseCursorTransform_.position);
		wvp *= orthoCamera_->GetVPMatrix();
		mouseCursorSprite_->CopyBufferData(0, &wvp, sizeof(Matrix4x4));
		mouseCursorSprite_->CopyBufferData(1, &mouseCursorTextureIndex_, sizeof(int));
	}

	if (keys[Key::Correct]) {
		switch (titleUI_->GetCurrentSelect()) {
		case Title::Select::Start:
			fadeManager_->StartFadeIn([]() { return std::make_unique<ShigeScene>(); });
			break;
		case Title::Select::Option:
			isOptionMode_ = true;
			break;
		case Title::Select::Quit:
			commonData_->shouldQuit = true;
			break;
		}
	}

	if (auto next = fadeManager_->TakeNextScene()) {
		return next;
	}

	return nullptr;
}

void TitleScene::Draw() {
	auto window = commonData_->mainWindow.second.get();
	auto display = commonData_->display.get();
	auto cmdObj = commonData_->cmdObject.get();

	// ディスプレイへの描画開始
	display->PreDraw(cmdObj, true);

	// Mapの描画（displayに描画）
	//map_->Draw(cmdObj);

	// yukaの描画
	yukaRender_->Draw(cmdObj);

	// グリッドの描画
	//grid_->Draw(cmdObj);

	// Playerの描画（displayに描画）
	player_->Draw(cmdObj);

	// ターゲットマーカーの描画
	if (isTargetMarkerVisible_) {
		targetMarkerRender_->Draw(cmdObj);
	}

	// TitleUIの描画（displayに描画）
	titleUI_->Draw(cmdObj);

	// フェードの描画
	fadeManager_->Draw(cmdObj);

	// フレームの描画
	gameFrame_->Draw(cmdObj);

	// マウスカーソルスプライトの描画
	mouseCursorSprite_->Draw(cmdObj);

	// ディスプレイへの描画終了
	display->PostDraw(cmdObj);

#ifdef SH_RELEASE
	postEffectConfig_.output = commonData_->mainWindow.second->GetCurrentDisplay();
	postEffect_->Draw(postEffectConfig_);

	// ウィンドウへの描画（displayの内容を転送）
	window->PreDraw(cmdObj, false);

#else

	// ウィンドウへの描画（displayの内容を転送）
	window->PreDraw(cmdObj, true);

#endif

	//ここ以外で記述する場合、ifdefを忘れないようにすること
#ifdef USE_IMGUI

	gameCamera_->DrawImGui();

	ImGui::Begin("Title Scene Settings");

	// カメラの設定
	if (ImGui::TreeNode("Camera")) {
		static Vector3 cameraPos = { 0.0f, 0.0f, -10.0f };
		static Vector3 cameraRot = { 0.0f, 0.0f, 0.0f };
		static Vector3 cameraScale = { 1.0f, 1.0f, 1.0f };

		if (ImGui::DragFloat3("Position", &cameraPos.x, 0.1f)) {
			camera_->SetPosition(cameraPos);
		}

		if (ImGui::DragFloat3("Rotation", &cameraRot.x, 0.01f)) {
			camera_->SetRotation(cameraRot);
		}

		if (ImGui::DragFloat3("Scale", &cameraScale.x, 0.01f, 0.01f, 10.0f)) {
			camera_->SetScale(cameraScale);
		}

		camera_->MakeMatrix();

		ImGui::TreePop();
	}

	// オプション設定の表示
	if (ImGui::TreeNode("Option Settings")) {
		ImGui::Checkbox("Is Option Mode", &isOptionMode_);

		if (isOptionMode_) {
			const char* optionSelectNames[] = { "Master", "BGM", "SE", "Quit" };
			int currentIndex = static_cast<int>(currentOptionSelect_);
			ImGui::Combo("Current Option Select", &currentIndex, optionSelectNames, static_cast<int>(Option::Select::Count));
		}

		ImGui::Separator();
		ImGui::Text("Volume Settings");
		ImGui::SliderFloat("Master Volume", masterVolume_, 0.0f, 1.0f);
		ImGui::SliderFloat("BGM Volume", bgmVolume_, 0.0f, 1.0f);
		ImGui::SliderFloat("SE Volume", seVolume_, 0.0f, 1.0f);

		ImGui::TreePop();
	}

	ImGui::End();

	// TitleUIの設定
	titleUI_->DrawImGui();

	// Mapの設定（SRT調整）
	map_->DrawDebugGUI();

	display->DrawImGui();

	ImGui::Begin("Depth");
	ImGui::Image((ImTextureRef)commonData_->display->GetDisplay()->GetDepthTexture()->GetGPUHandle().ptr, ImVec2(256, 9 * 16));
	ImGui::End();

	// プレイヤーのデバッグ情報を表示
	if (player_) {
		player_->DrawImGui();
	}

	ImGui::Begin("FPS");
	float deltaTime = engine_->GetFPSObserver()->GetDeltatime();
	ImGui::Text("DeltaTime: %.3f ms", deltaTime * 1000.0f);
	ImGui::Text("FPS: %.1f", 1.0f / deltaTime);
	ImGui::End();
#endif

	engine_->DrawImGui();

	// ウィンドウへの描画終了
	window->ToPresent(cmdObj);
}

void TitleScene::UpdateCalculatedVolumes() {
	calculatedBgmVolume_ = (*bgmVolume_) * (*masterVolume_);
	calculatedSeVolume_ = (*seVolume_) * (*masterVolume_);
}
