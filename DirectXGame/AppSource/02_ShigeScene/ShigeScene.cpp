#include "ShigeScene.h"
#include "ShopScene.h"
#include <Common/KeyConfig/WorldCursor.h>
#include <Utility/Color.h>
#include <format>
#include <imgui/imgui.h>
#include <numbers>
#include <windows.h>

void ShigeScene::Initialize() {
	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize(input_);

	gameCamera_ = std::make_unique<GameCamera>();
	gameCamera_->Initialize();

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

	enemyManager_ = std::make_unique<EnemyManager>();
	enemyManager_->Initialize(player_->GetPositionPtr());
	IEnemy::SetModelManager(modelManager_);
	IEnemy::SetDrawDataManager(drawDataManager_);

	map_ = std::make_unique<Map>();
	map_->Initialize(drawDataManager_, modelManager_);
	player_->SetMapInfo(map_->GetMapInfo());

	objectRender_ = std::make_unique<ObjectRender>();
	objectRender_->Initialize(drawDataManager_, modelManager_);

	weaponDatabase_ = std::make_unique<WeaponDatabase>();
	weaponDatabase_->Initialize(jsonManager_);

	attackManager_ = std::make_unique<AttackManager>();
	attackManager_->Initialize(modelManager_);

	IWeapon::StaticInitialize(attackManager_.get(), enemyManager_.get(), weaponDatabase_.get());

	waveSystem_ = std::make_unique<LevelSystem>();
	waveSystem_->Initialize(enemyManager_.get(), commonData_->stageNum++, player_->GetPositionPtr(), map_->GetMapInfo());

	gameTimer_ = std::make_unique<GameTimer>();
	gameTimer_->Initialize();

	aiController_ = std::make_unique<AIController>(player_->GetPositionPtr(), enemyManager_.get());
	inputController_ = std::make_unique<InputController>(input_);
	controllers_.push_back(aiController_.get());
	controllers_.push_back(inputController_.get());
	currentControllerIndex_ = 0;
	player_->SetController(controllers_[currentControllerIndex_]); // AIコントローラーを適用

	orthoCamera_ = std::make_unique<Camera>();

	parameterRender_ = std::make_unique<ParameterRender>();
	parameterRender_->Initialize(modelManager_, drawDataManager_, engine_);

	gameFrame_ = std::make_unique<GameFrame>();
	SHEngine::DrawData planeDrawData = drawDataManager_->GetDrawData(modelManager_->GetNodeModelData(1).drawDataIndex);
	gameFrame_->Initialize(planeDrawData, textureManager_->LoadTexture("Frame.png"));

	postEffect_ = std::make_unique<PostEffect>();
	SHEngine::DrawData postEffectDrawData = drawDataManager_->GetDrawData(commonData_->postEffectDrawDataIndex);
	postEffect_->Initialize(textureManager_, postEffectDrawData);
	postEffectConfig_.cmdObj = commonData_->cmdObject.get();
	postEffectConfig_.origin = commonData_->display->GetDisplay();
	postEffectConfig_.jobs_ = 0;

	timerText_ = std::make_unique<SHEngine::Text>(64);
	timerText_->Initialize(planeDrawData, "YDWbananaslipplus.otf", 64);
	timerText_->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });

	timerTextTransform_.position = { 775.0f, -295.0f, 0.0f }; // Top center or so // default
	timerTextTransform_.scale = { 1.f, 1.f, 1.0f };

	displayRange_.top = 390.0f;
	displayRange_.bottom = 810.0f;
	displayRange_.left = 560.0f;
	displayRange_.right = 1340.0f;

//#ifdef USE_IMGUI
//
//	displayRange_.top = 165.0f;
//	displayRange_.bottom = 375.0f;
//	displayRange_.left = 240.0f;
//	displayRange_.right = 630.0f;
//
//#endif // DEBUG

}

std::unique_ptr<IScene> ShigeScene::Update() {

	MakeWeapon();

	float deltaTime = engine_->GetFPSObserver()->GetDeltatime();
	shopScene_->SetDeltaTime(deltaTime);
	shopScene_->Update();

	gameFrame_->Update();

	Vector2 cursorPos = commonData_->keyManager->GetCursorPos();
	bool inDisplayRange = false;
	if (cursorPos.x >= displayRange_.left && cursorPos.x <= displayRange_.right &&
	    cursorPos.y >= displayRange_.top && cursorPos.y <= displayRange_.bottom) {
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
	Vector3 cameraPos = {0.f, 0.f, 0.f};
	grid_->Update(cameraPos, camera_->GetVPMatrix());
	auto key = commonData_->keyManager->GetKeyStates();

	gameTimer_->Update(deltaTime);
	waveSystem_->Update(deltaTime);

	std::wstring timerWStr = std::format(L"{:.0f}", gameTimer_->GetTimer());
	timerText_->SetText(timerWStr);

	if (key[Key::ControllerChange]) {
		// インデックスを切り替える
		currentControllerIndex_ = (currentControllerIndex_ + 1) % controllers_.size();

		// プレイヤーに新しいコントローラーをセット
		player_->SetController(controllers_[currentControllerIndex_]);
	}

	{
		// マウスクリックによる敵のターゲット選択
		if (key[Key::Target]) {

			// マウスのカーソル座標を取得してワールド座標に変換
			Vector2 cursorPos = commonData_->keyManager->GetCursorPos();
			Vector3 clickWorldPos = GetWorldCursor(camera_, cursorPos);

			IEnemy* clickedEnemy = nullptr;
			float minClickDist = FLT_MAX;
			float clickHitRadius = 1.0f; // クリック判定の大きさ

			// 敵のリストを調べて、クリックされた座標に一番近い敵を探す
			for (IEnemy* enemy : enemyManager_->GetEnemies()) {
				if (!enemy->IsActive())
					continue;

				float dx = enemy->GetPosition().x - clickWorldPos.x;
				float dz = enemy->GetPosition().z - clickWorldPos.z;
				float dist = std::sqrtf(dx * dx + dz * dz);

				// クリック範囲内にいて、かつ一番近い敵を選ぶ
				if (dist < clickHitRadius && dist < minClickDist) {
					minClickDist = dist;
					clickedEnemy = enemy;
				}
			}

			// AIController経由でPlayerAIにターゲットを設定する
			if (aiController_) {
				// ターゲットの切り替えと解除の処理
				IEnemy* currentTarget = aiController_->GetTargetEnemy();

				if (clickedEnemy) {
					if (clickedEnemy == currentTarget) {
						// すでにターゲットしている敵をもう一度クリックしたら解除
						aiController_->SetTargetEnemy(nullptr);
					} else {
						// 別の敵をクリックしたら新しいターゲットに設定
						aiController_->SetTargetEnemy(clickedEnemy);
					}
				} else {
					// 敵以外の場所をクリックした場合も解除
					aiController_->SetTargetEnemy(nullptr);
				}
			}
		}
	}

	player_->Update(camera_->GetVPMatrix(), deltaTime, key);
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

	parameterRender_->Update(orthoCamera_->GetVPMatrix(), player_->GetParameters(), deltaTime, key);
	map_->Update(camera_->GetVPMatrix());
	enemyManager_->Update(deltaTime, camera_->GetVPMatrix(), orthoCamera_->GetVPMatrix());
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

		objectRender_->SetDrawInfo(drawInfos_.data(), drawInfos_.size(), camera_->GetVPMatrix());
	}

	{
		int weaponCount = static_cast<int>(weaponRenders_.size());
		for (size_t i = 0; i < weaponCount; ++i) {
			// 武器が1つ以上のときだけ計算
			if (weaponCount > 0) {
				// プレイヤー座標にオフセットを加算
				Vector3 weaponPos = player_->GetTransform().position;

				weaponRenders_[i]->Update(camera_->GetVPMatrix(), weaponPos, deltaTime);
			}
		}
	}

	if (key[Key::Debug1] || gameTimer_->IsEnd()) {
	}

	if (player_->GetCurrentHP() <= 0) {
		std::string debugMsg = std::format("Player Survived Time: {:.2f} s\n", gameTimer_->GetTimer());
		OutputDebugStringA(debugMsg.c_str());
		return std::make_unique<TitleScene>();
	}

	return nullptr;
}

void ShigeScene::Draw() {
	auto window = commonData_->mainWindow.second.get();
	auto display = commonData_->display.get();
	auto cmdObj = commonData_->cmdObject.get();

	shopScene_->DrawReady();

	display->PreDraw(cmdObj, true);

	// grid_->Draw(cmdObj);
	map_->Draw(cmdObj);
	objectRender_->Draw(cmdObj);
	player_->Draw(cmdObj);
	playerHP_->Draw(cmdObj);

	parameterRender_->Draw(cmdObj);

	waveSystem_->DrawImGui();

	for (const auto& render : weaponRenders_) {
		render->Draw(cmdObj);
	}

	timerText_->Draw(cmdObj);

	shopScene_->Draw();

	gameFrame_->Draw(cmdObj);

	display->PostDraw(cmdObj);

	window->PreDraw(cmdObj);

	// ここ以外で記述する場合、ifdefを忘れないようにすること
#ifdef USE_IMGUI

	display->DrawImGui();

	ImGui::Begin("Game Timer");
	ImGui::Text("Game Time : %.2f s", gameTimer_->GetTimer());
	ImGui::DragFloat3("Pos", &timerTextTransform_.position.x, 1.0f);
	ImGui::DragFloat3("Scale", &timerTextTransform_.scale.x, 0.01f);
	ImGui::End();

	ImGui::Begin("FPS");
	float deltaTime = engine_->GetFPSObserver()->GetDeltatime();
	ImGui::Text("DeltaTime: %.3f ms", deltaTime * 1000.0f);
	ImGui::Text("FPS: %.1f", 1.0f / deltaTime);
	ImGui::End();

	ImGui::Begin("RenderDebug");
	ImGui::DragFloat("baseHeight", &baseHeight_, 0.01f);
	ImGui::DragFloat("baseRadius", &baseRadius_, 0.01f);
	ImGui::End();

	ImGui::Begin("KeyInfo");
	ImGui::Text("1 : HP減少");
	ImGui::Text("2 : HP回復");
	ImGui::Text("3 : HP全回復");
	ImGui::Text("4 : HPゼロ");
	ImGui::Text("5 : 自動と手動の切り換え");
	ImGui::Text("%s", currentControllerIndex_ == 0 ? "AIController" : "InputController");
	ImGui::End();

	camera_->DrawImGui();
	shopScene_->GetCamera()->DrawImGui();

#endif

	engine_->DrawImGui();

#ifdef SH_RELEASE
	postEffectConfig_.output = commonData_->mainWindow.second->GetCurrentDisplay();
	postEffect_->Draw(postEffectConfig_);
#endif

	window->PostDraw(cmdObj);
}

void ShigeScene::MakeWeapon() {
	for (const auto& piece : commonData_->pieces) {
		// 作成済みかどうか確認
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

			switch (data->type) {
			case WeaponType::Pistol: {
				weapon = std::make_unique<Pistol>();
				break;
			}
			case WeaponType::Sword: {
				weapon = std::make_unique<Sword>();
				break;
			}
			case WeaponType::ShotGun: {
				weapon = std::make_unique<ShotGun>();
				break;
			}
			case WeaponType::Spear: {
				weapon = std::make_unique<Spear>();
				break;
			}
			case WeaponType::Axe: {
				weapon = std::make_unique<Ax>();
				break;
			}
			case WeaponType::Fist: {
				weapon = std::make_unique<Fist>();
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
			}

			weaponRender->Initialize(drawDataManager_, modelManager_, textureManager_, weapon.get(), piece->GetItem());
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
