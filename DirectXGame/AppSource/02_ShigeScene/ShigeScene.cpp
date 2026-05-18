#include "ShigeScene.h"
#include "ShopScene.h"
#include <../Engine/Assets/Audio/AudioManager.h>
#include <Common/KeyConfig/WorldCursor.h>
#include <Utility/Color.h>
#include <Utility/Matrix.h>
#include <Utility/MatrixFactory.h>
#include <format>
#include <imgui/imgui.h>
#include <numbers>
#include <windows.h>

ShigeScene::~ShigeScene() {
	bgm_->Stop();
}

void ShigeScene::Initialize() {
	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize(input_);

	gameCamera_ = std::make_unique<GameCamera>();
	gameCamera_->Initialize();
	gameCamera_->SetInput(input_);
	gameCamera_->SetOffset({ 0.0f, 30.0f, -45.0f });
	gameCamera_->Setrotation({ -0.5f, 0.0f, 0.0f });

	camera_ = gameCamera_.get();

	shopScene_ = std::make_unique<ShopScene>();
	shopScene_->Ready(engine_, commonData_);
	shopScene_->Initialize();

	grid_ = std::make_unique<Grid>();
	grid_->Initialize(drawDataManager_);

	colliderManager_ = std::make_unique<ColliderManager>();
	Collider::SetColliderManager(colliderManager_.get());

	playerHP_ = std::make_unique<Player::HP>();
	playerHP_->Initialize(modelManager_, drawDataManager_);

	player_ = std::make_unique<Player::Base>();
	player_->Initialize(modelManager_, drawDataManager_, CharacterID::Warrior, shopScene_->GetItemManager());
	player_->UpdateParameter(commonData_->pieces);

	map_ = std::make_unique<Map>();
	map_->Initialize(drawDataManager_, modelManager_, {}, "Assets/Model/battleStage");
	player_->SetMapInfo(map_->GetMapInfo());

	enemyEffectManager_ = std::make_unique<EnemyEffect>();
	enemyEffectManager_->Initialize(textureManager_, modelManager_, commonData_);

	SHEngine::DrawData planeDrawData = drawDataManager_->GetDrawData(modelManager_->GetNodeModelData(1).drawDataIndex);
	enemyManager_ = std::make_unique<EnemyManager>();
	enemyManager_->Initialize(player_->GetPositionPtr(), map_.get(), planeDrawData, modelManager_, enemyEffectManager_.get());
	IEnemy::SetModelManager(modelManager_);
	IEnemy::SetDrawDataManager(drawDataManager_);

	objectRender_ = std::make_unique<ObjectRender>();
	objectRender_->Initialize(drawDataManager_, modelManager_);

	weaponDatabase_ = std::make_unique<WeaponDatabase>();
	weaponDatabase_->Initialize(jsonManager_);

	attackManager_ = std::make_unique<AttackManager>();
	attackManager_->Initialize(modelManager_);
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

	IWeapon::StaticInitialize(attackManager_.get(), enemyManager_.get(), weaponDatabase_.get());

	waveSystem_ = std::make_unique<LevelSystem>();
	waveSystem_->Initialize(enemyManager_.get(), player_->GetPositionPtr(), map_->GetMapInfo());

	waveSystemUI_ = std::make_unique<LevelSystemUI>();
	waveSystemUI_->Initialize(modelManager_, drawDataManager_, textureManager_);

	gameTimer_ = std::make_unique<GameTimer>();
	gameTimer_->Initialize();

	aiController_ = std::make_unique<AIController>(player_->GetPositionPtr(), enemyManager_.get());
	inputController_ = std::make_unique<InputController>(input_);
	inputController_->SetCurrentPosition(player_->GetPositionPtr());
	inputController_->SetFallbackController(aiController_.get()); // AIをフォールバックに設定

	controllers_.push_back(aiController_.get());
	controllers_.push_back(inputController_.get());
	currentControllerIndex_ = 1; // InputController(ハイブリッド動作)を設定
	player_->SetController(controllers_[currentControllerIndex_]);

	orthoCamera_ = std::make_unique<Camera>();

	parameterRender_ = std::make_unique<ParameterRender>();
	parameterRender_->Initialize(modelManager_, drawDataManager_, engine_);

	gameFrame_ = std::make_unique<GameFrame>();
	gameFrame_->Initialize(planeDrawData, textureManager_->LoadTexture("Frame2.png"));

	gameFrameBG_ = std::make_unique<GameFrame>();
	gameFrameBG_->Initialize(planeDrawData, textureManager_->LoadTexture("FrameBG2.png"));

	gameDisplay_ = std::make_unique<ShopDisplay>();
	gameDisplay_->Initialize(commonData_->cmdObject.get(), planeDrawData, textureManager_);
	gameDisplay_->SetTransform({450.0f, 256.0f}, {784.0f, 416.0f});

	postEffect_ = std::make_unique<PostEffect>();
	//Post Effect Draw Data
	auto pedd = drawDataManager_->GetDrawData(commonData_->postEffectDrawDataIndex);
	postEffect_->Initialize(textureManager_, pedd, true);
	postEffectConfig_.cmdObj = commonData_->cmdObject.get();
	postEffectConfig_.origin = commonData_->display->GetDisplay();

	timerText_ = std::make_unique<SHEngine::Text>(64);
	timerText_->Initialize(planeDrawData, "YDWbananaslipplus.otf", 64);
	timerText_->SetColor({1.0f, 1.0f, 1.0f, 1.0f});

	timerTextTransform_.position = {550.0f, -85.0f, 0.0f}; // Top center or so // default
	timerTextTransform_.scale = {2.0f, 2.0f, 1.0f};

	enemySpawnGraphText_ = std::make_unique<SHEngine::Text>(64);
	enemySpawnGraphText_->Initialize(planeDrawData, "YDWbananaslipplus.otf", 64);
	enemySpawnGraphText_->SetColor({1.0f, 1.0f, 1.0f, 1.0f});

	// 1280x720の画面内座標系での正しい範囲に修正
	displayRange_.top = 280.0f;
	displayRange_.bottom = 685.0f;
	displayRange_.left = 450.0f;
	displayRange_.right = 1230.0f;

	// #ifdef USE_IMGUI
	//
	//	displayRange_.top = 165.0f;
	//	displayRange_.bottom = 375.0f;
	//	displayRange_.left = 240.0f;
	//	displayRange_.right = 630.0f;
	//
	//
	// #endif // DEBUG

	bgmVolume_ = commonData_->bgmVolume* commonData_->masterVolume;
	seVolume_ = commonData_->seVolume * commonData_->masterVolume;

	// BGM
	auto data = AudioManager::GetInstance()->GetData("GameScene.mp3");
	data->SetVolume(0);
	bgm_ = data->CustomPlay(255);

	// ライトの設定
	dirLight_.color = {1.0f, 1.0f, 1.0f, 1.0f};
	dirLight_.direction = {0.0f, 1.0f, 0.0f};
	dirLight_.intensity = 1.0f;

	flashEffect_ = std::make_unique<Flash>();
	flashEffect_->Initialize(modelManager_, drawDataManager_);

	letterBox_ = std::make_unique<LetterBox>();
	letterBox_->Initialize(modelManager_, drawDataManager_);

	fadeManager_ = std::make_unique<FadeManager>();
	fadeManager_->Initialize(modelManager_, drawDataManager_);
	fadeManager_->StartFadeOut(false);
}

std::unique_ptr<IScene> ShigeScene::Update() {

	if (input_->GetKeyState(DIK_TAB) && !input_->GetPreKeyState(DIK_TAB)) {
		return std::make_unique<TitleScene>();
	}

	MakeWeapon();

	float deltaTime = engine_->GetFPSObserver()->GetDeltatime();
	shopScene_->SetDeltaTime(deltaTime);
	shopScene_->Update();

	gameFrameBG_->Update();
	gameFrame_->Update();

	gameDisplay_->Update();
	Vector2 cursorPos = commonData_->keyManager->GetCursorPos();
	bool inDisplayRange = false;
	if (cursorPos.x >= displayRange_.left && cursorPos.x <= displayRange_.right && cursorPos.y >= displayRange_.top && cursorPos.y <= displayRange_.bottom) {
		inDisplayRange = true;
	}

	bool isRightClickHeld = (input_->GetMouseButtonState()[1] & 0x80) != 0;

	if (inDisplayRange && isRightClickHeld && !isCameraDragging_) {
		isCameraDragging_ = true;
	}

	if (isCameraDragging_) {
		if (isRightClickHeld) {
			Vector2 delta = input_->GetMouseMove();
			float moveScale = 0.05f;
			cameraTargetOffset_.x -= delta.x * moveScale;
			cameraTargetOffset_.z += delta.y * moveScale;
		} else {
			isCameraDragging_ = false;
			cameraTargetOffset_ = {0.0f, 0.0f, 0.0f};
		}
	} else {
		cameraTargetOffset_ = {0.0f, 0.0f, 0.0f};
	}

	Vector3 cameraTargetPos = player_->GetTransform().position + cameraTargetOffset_;

	gameCamera_->Update(deltaTime, cameraTargetPos);
	Vector3 cameraPos = gameCamera_->GetPosition();
	grid_->Update(cameraPos, camera_->GetVPMatrix());
	auto key = commonData_->keyManager->GetKeyStates();

	gameTimer_->Update(deltaTime);
	waveSystem_->Update(deltaTime);

	float time = gameTimer_->GetTimer();
	int minutes = static_cast<int>(time) / 60;
	int seconds = static_cast<int>(time) % 60;
	std::wstring timerWStr = std::format(L"{:d}:{:02d}", minutes, seconds);
	timerText_->SetText(timerWStr);

	{
		// マウスクリックによる敵のターゲット選択
		if (key[Key::Target]) {

			// マウスのカーソル座標を取得
			Vector2 cursorPos = commonData_->keyManager->GetCursorPos();

			// GameDisplayの内側でのみPlayerを移動させる
			if (cursorPos.x >= displayRange_.left && cursorPos.x <= displayRange_.right && cursorPos.y >= displayRange_.top && cursorPos.y <= displayRange_.bottom) {

				// displayRange内の相対座標を計算 (0.0～1.0の範囲)
				float relativeX = (cursorPos.x - displayRange_.left) / (displayRange_.right - displayRange_.left);
				float relativeY = (cursorPos.y - displayRange_.top) / (displayRange_.bottom - displayRange_.top);

				// 元の1280x720の座標系に逆変換
				Vector2 originalScreenPos;
				originalScreenPos.x = relativeX * 1280.0f;
				originalScreenPos.y = relativeY * 720.0f;

				// ワールド座標に変換
				Vector3 clickWorldPos = GetWorldCursor(camera_, originalScreenPos);

				// マップ境界内に制限した座標を取得
					Vector3 clampedPos = map_->ClampToBounds(clickWorldPos);
					// Playerと同じXZ円形範囲内に制限する
					{
						const MapInfo& mapInfo = map_->GetMapInfo();
						float dx = clampedPos.x - mapInfo.centerX;
						float dz = clampedPos.z - mapInfo.centerZ;
						float dist = std::sqrt(dx * dx + dz * dz);
						if (dist > mapInfo.radius) {
							float ratio = mapInfo.radius / dist;
							clampedPos.x = mapInfo.centerX + dx * ratio;
							clampedPos.z = mapInfo.centerZ + dz * ratio;
						}
					}
					// Playerを指定のワールド座標へ移動させる（InputController使用時用）
					player_->GetController()->SetTargetPosition(clampedPos);
			}
		}
	}

	player_->SetDirectionalLight(dirLight_);
	player_->Update(camera_->GetVPMatrix(), deltaTime, key);

	// ターゲットマーカーの更新
	if (player_->GetController()->HasTarget()) {
		isTargetMarkerVisible_ = true;
		targetMarkerTransform_.position = player_->GetController()->GetTargetPosition();
		// Zファイティング（地面とのチラつき）を防ぐために少しY座標を浮かせる
		targetMarkerTransform_.position.y += 0.02f;

		targetMarkerAnimTimer_ += deltaTime;
		// XZに拡縮 (点滅のように見えないよう、速度を少し緩やかに調整)
		float scale = 1.0f + 0.2f * std::sin(targetMarkerAnimTimer_ * 5.0f);
		targetMarkerTransform_.scale = {scale, 1.0f, scale}; // Y軸方向は拡縮させない

		Matrix4x4 wvp = Matrix::MakeScaleMatrix(targetMarkerTransform_.scale) * Matrix::MakeRotationMatrix(targetMarkerTransform_.rotate) *
		                Matrix::MakeTranslationMatrix(targetMarkerTransform_.position) * camera_->GetVPMatrix();

		Vector4 markerColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		targetMarkerRender_->CopyBufferData(0, &wvp, sizeof(wvp));
		targetMarkerRender_->CopyBufferData(1, &markerColor, sizeof(markerColor));
		targetMarkerRender_->CopyBufferData(2, &targetMarkerTexIndex_, sizeof(targetMarkerTexIndex_));
		targetMarkerRender_->CopyBufferData(3, &dirLight_, sizeof(DirectionalLight));
	} else {
		isTargetMarkerVisible_ = false;
	}

	player_->UpdateParameter(commonData_->pieces);
	playerHP_->Update(orthoCamera_->GetVPMatrix(), deltaTime, player_->GetCurrentHP(), player_->GetMaxHP());

	OrthographicDesc orthDesc;
	orthDesc.SetValue();
	orthoCamera_->SetProjectionMatrix(orthDesc);
	orthoCamera_->SetScale({1, -1, 1});
	orthoCamera_->SetPosition({0, 0, 0});
	orthoCamera_->MakeMatrix();

	timerText_->Update(orthoCamera_->GetVPMatrix());
	timerText_->SetTransform(timerTextTransform_);

	enemySpawnGraphText_->Update(orthoCamera_->GetVPMatrix());
	enemySpawnGraphText_->SetTransform(enemySpawnGraphTextTransform_);
	enemySpawnGraphText_->SetText(L"5分間生き残れ！");

	parameterRender_->Update(orthoCamera_->GetVPMatrix(), player_->GetParameters(), deltaTime, key);
	map_->Update(camera_->GetVPMatrix());
	enemyManager_->Update(deltaTime, camera_->GetVPMatrix(), orthoCamera_->GetVPMatrix());
	enemyEffectManager_->Update(deltaTime);
	enemyEffectManager_->SetCameraPos(cameraPos);
	for (const auto& weapon : weapons_) {
		weapon->Update(deltaTime);
	}
	attackManager_->Update(deltaTime);

	colliderManager_->CollisionCheckAll();

	// DrawInfoを収集して描画クラスに渡す
	{
		drawInfos_.clear();
		auto enemyDI = enemyManager_->GetEnemyDrawInfos();
		drawInfos_.insert(drawInfos_.end(), enemyDI.begin(), enemyDI.end());
		auto attackDI = attackManager_->GetAttackDrawInfos();
		drawInfos_.insert(drawInfos_.end(), attackDI.begin(), attackDI.end());

		objectRender_->SetDirectionalLight(dirLight_);
		objectRender_->SetDrawInfo(drawInfos_.data(), drawInfos_.size(), camera_->GetVPMatrix());
	}

	{
		int weaponCount = static_cast<int>(weaponRenders_.size());
		for (size_t i = 0; i < weaponCount; ++i) {
			// 武器が1つ以上のときだけ計算
			if (weaponCount > 0) {
				// プレイヤー座標にオフセットを加算
				Vector3 weaponPos = player_->GetTransform().position;

				weaponRenders_[i]->SetDirectionalLight(dirLight_);
				weaponRenders_[i]->Update(camera_->GetVPMatrix(), weaponPos, deltaTime);
			}
		}
	}

	waveSystem_->Update(deltaTime);
	waveSystemUI_->Update(*waveSystem_, orthoCamera_->GetVPMatrix(), deltaTime);

	if (key[Key::Debug1] || gameTimer_->IsEnd()) {
	}

	flashEffect_->Update(orthoCamera_->GetVPMatrix(), deltaTime);
	letterBox_->Update(orthoCamera_->GetVPMatrix(), deltaTime);

	if (player_->GetCurrentHP() <= 0) {
		if (!flashEffect_->GetIsActive() && !isPlayerDead_) {
			flashEffect_->Trigger();
			isPlayerDead_ = true;
		}

		if (isPlayerDead_) {
			if (flashEffect_->GetIntensity() <= 0.0f && flashEffect_->GetIsActive() && !letterBox_->GetIsTriggered()) {
				letterBox_->Trigger();
			}

			if (letterBox_->GetIsTriggered() && !letterBox_->GetIsActive()) {
				std::string debugMsg = std::format("Player Survived Time: {:.2f} s\n", gameTimer_->GetTimer());
				OutputDebugStringA(debugMsg.c_str());
				commonData_->isWin = false;
				commonData_->clearTime = gameTimer_->GetTimer();
				commonData_->killCount = enemyManager_->GetKillCount();
 				fadeManager_->StartFadeIn();
			}
		}
	} else if (waveSystem_->End()) {
		commonData_->isWin = true;
		commonData_->clearTime = gameTimer_->GetTimer();
		commonData_->killCount = enemyManager_->GetKillCount();
		fadeManager_->StartFadeIn();
	}

	fadeManager_->Update(camera_->GetVPMatrix(), deltaTime);

	if(fadeManager_->Finished()){
		return std::make_unique<ResultScene>();
	}

	commonData_->weaponCount = weaponRenders_.size();
	commonData_->enemyCount = enemyManager_->GetEnemies().size();

	return nullptr;
}

void ShigeScene::Draw() {
	auto window = commonData_->mainWindow.second.get();
	auto display = commonData_->display.get();
	auto cmdObj = commonData_->cmdObject.get();

	shopScene_->DrawReady();

	// GameSceneの描画
	gameDisplay_->PreDraw();

	// grid_->Draw(cmdObj);
	map_->Draw(cmdObj);
	objectRender_->Draw(cmdObj);

	if (isTargetMarkerVisible_ && targetMarkerRender_) {
		targetMarkerRender_->Draw(cmdObj);
	}

	player_->Draw(cmdObj);
	playerHP_->Draw(cmdObj);

	waveSystem_->DrawImGui();

	for (const auto& render : weaponRenders_) {
		render->Draw(cmdObj);
	}

	enemyManager_->Draw(cmdObj);
	//enemyEffectManager_->Draw();

	commonData_->trailDrawer->Draw(cmdObj, camera_->GetVPMatrix());
	commonData_->particleDrawer->Draw(cmdObj, camera_->GetVPMatrix());

	controllers_[0]->DrawImGui();

	timerText_->Draw(cmdObj);

	enemySpawnGraphText_->Draw(cmdObj);

	parameterRender_->Draw(cmdObj);

	flashEffect_->Draw(cmdObj);

	letterBox_->Draw(cmdObj);

	gameDisplay_->ToPresent();

	// 画面全体の描画
	display->PreDraw(cmdObj, true);

	gameFrameBG_->Draw(cmdObj);

	shopScene_->Draw();

	gameDisplay_->Draw();

	waveSystemUI_->Draw(cmdObj);

	gameFrame_->Draw(cmdObj);

	fadeManager_->Draw(cmdObj);

	display->PostDraw(cmdObj);

	postEffectConfig_.output = commonData_->mainWindow.second->GetCurrentDisplay();
	postEffect_->Draw(postEffectConfig_);

	window->PreDraw(cmdObj, false);

	// ここ以外で記述する場合、ifdefを忘れないようにすること
#ifdef USE_IMGUI

	display->DrawImGui();

	waveSystemUI_->DrawImGui();

	TackleEnemy::DrawImGui();

	map_->DrawDebugGUI();

	ImGui::Begin("Game Timer");
	ImGui::Text("Game Time : %.2f s", gameTimer_->GetTimer());
	ImGui::DragFloat3("Pos", &timerTextTransform_.position.x, 1.0f);
	ImGui::DragFloat3("Scale", &timerTextTransform_.scale.x, 0.01f);
	ImGui::End();

	/*ImGui::Begin("敵出現量グラフ");
	ImGui::DragFloat3("Text Pos", &enemySpawnGraphTextTransform_.position.x, 1.0f);
	ImGui::DragFloat3("Text Scale", &enemySpawnGraphTextTransform_.scale.x, 0.01f);

	Vector4 textColor = { 1.0f, 0.5f, 0.2f, 1.0f };
	if (ImGui::ColorEdit4("Text Color", &textColor.x)) {
	    enemySpawnGraphText_->SetColor(textColor);
	}
	ImGui::End();*/

	ImGui::Begin("FPS");
	float deltaTime = engine_->GetFPSObserver()->GetDeltatime();
	ImGui::Text("DeltaTime: %.3f ms", deltaTime * 1000.0f);
	ImGui::Text("FPS: %.1f", 1.0f / deltaTime);
	ImGui::End();

	ImGui::Begin("RenderDebug");
	ImGui::DragFloat("baseHeight", &baseHeight_, 0.01f);
	ImGui::DragFloat("displayLeft", &displayRange_.left, 1.0f);
	ImGui::DragFloat("displayRight", &displayRange_.right, 1.0f);
	ImGui::DragFloat("displayTop", &displayRange_.top, 1.0f);
	ImGui::DragFloat("displayBottom", &displayRange_.bottom, 1.0f);
	ImGui::End();

	ImGui::Begin("Light");
	ImGui::DragFloat3("Direction", &dirLight_.direction.x, 0.01f);
	ImGui::DragFloat("intensity", &dirLight_.intensity, 0.01f);
	dirLight_.direction = dirLight_.direction.Normalize();
	ImGui::End();

	// プレイヤーのデバッグ情報を表示
	if (player_) {
		player_->DrawImGui();
	}

#endif // USE_IMGUI
	engine_->DrawImGui();

	window->ToPresent(cmdObj);
}

void ShigeScene::MakeWeapon() {
	for (const auto& piece : commonData_->pieces) {
		// 作成済みかどうか確認.
		{
			bool found = false;
			for (const auto& weapon : weapons_) {
				if (weapon->GetPiecePtr() == piece) {
					found = true;
					break;
				}
			}

			if (found) {
				continue;
			}
		}

		int weaponID = piece->GetItem().weaponID;

		if (weaponID != -1) {
			WeaponData* data = weaponDatabase_->GetWeapon(weaponID);
			std::unique_ptr<IWeapon> weapon;
			std::unique_ptr<IWeaponRender> weaponRender = std::make_unique<IWeaponRender>();
			std::string name = "";

			switch (data->type) {
			case WeaponType::Pistol: {
				weapon = std::make_unique<Pistol>();
				break;
			}
			case WeaponType::Sword: {
				weapon = std::make_unique<Sword>();
				name = "Sword_Ribbon";
				break;
			}
			case WeaponType::ShotGun: {
				weapon = std::make_unique<ShotGun>();
				break;
			}
			case WeaponType::Spear: {
				weapon = std::make_unique<Spear>();
				name = "Spear_Ribbon";
				break;
			}
			case WeaponType::Axe: {
				weapon = std::make_unique<Ax>();
				name = "Axe_Ribbon3";
				break;
			}
			case WeaponType::Fist: {
				weapon = std::make_unique<Fist>();
				name = "Fist_Ribbon";
				break;
			}
			case WeaponType::Bow: {
				weapon = std::make_unique<Bow>();
				break;
			}
			case WeaponType::Gurepon: {
				weapon = std::make_unique<Gurepon>();
				break;
			}
			case WeaponType::Pickaxe: {
				weapon = std::make_unique<Pickaxe>();
				name = "Fist_Ribbon";
				break;
			}
			case WeaponType::Shuriken: {
				weapon = std::make_unique<Shuriken>();
				break;
			}
			}

			weaponRender->Initialize(drawDataManager_, modelManager_, textureManager_, weapon.get(), piece->GetItem(), name, *commonData_);
			weaponRenders_.push_back(std::move(weaponRender));

			weapon->Initialize(weaponID, player_.get());
			weapon->SetPiecePtr(piece);
			weapons_.push_back(std::move(weapon));
		}
	}

	// Pieceから削除された武器を削除する
	for (int i = 0; i < int(weaponRenders_.size()); ++i) {
		auto& wr = weaponRenders_[i];
		if (std::find_if(commonData_->pieces.begin(), commonData_->pieces.end(), [&](const auto& p) { return wr->GetPiecePtr() == p; }) == commonData_->pieces.end()) {
			wrDeleting_.push_back(std::make_pair(0, std::move(wr)));
			weaponRenders_.erase(weaponRenders_.begin() + i);
		}
	}

	weapons_.erase(
	    std::remove_if(
	        weapons_.begin(), weapons_.end(),
	        [&](const auto& w) {
		        // Piece がまだ存在するかチェック
		        return std::none_of(commonData_->pieces.begin(), commonData_->pieces.end(), [&](const auto& p) { return w->GetPiecePtr() == p; });
	        }),
	    weapons_.end());

	// 削除予定の武器描画オブジェクトを更新して削除
	for (auto& wrd : wrDeleting_) {
		wrd.first++;
		if (wrd.first > 5) { // 5フレーム後に完全に削除
			wrd.second.reset();
		}
	}
}
