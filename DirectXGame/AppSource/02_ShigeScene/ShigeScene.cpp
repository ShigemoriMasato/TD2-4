#include "ShigeScene.h"
#include "ShopScene.h"
#include <Utility/Color.h>
#include <imgui/imgui.h>
#include <numbers>

void ShigeScene::Initialize() {
	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize(input_);

	gameCamera_ = std::make_unique<GameCamera>();
	gameCamera_->Initialize();

	camera_ = gameCamera_.get();

	grid_ = std::make_unique<Grid>();
	grid_->Initialize(drawDataManager_);

	colliderManager_ = std::make_unique<ColliderManager>();
	Collider::SetColliderManager(colliderManager_.get());

	itemManager_ = std::make_unique<ItemManager>();
	itemManager_->Initialize(modelManager_);

	pieces_.reserve(commonData_->pieces.size());
	for (const auto& piece : commonData_->pieces) {
		pieces_.push_back(piece.get());
	}

	player_ = std::make_unique<Player::Base>();
	player_->Initialize(modelManager_, drawDataManager_, input_, CharacterID::Warrior, itemManager_.get());
	player_->UpdateParameter(pieces_);

	enemyManager_ = std::make_unique<EnemyManager>();
	enemyManager_->Initialize(player_->GetPositionPtr());
	IEnemy::SetModelManager(modelManager_);

	map_ = std::make_unique<Map>();
	map_->Initialize(drawDataManager_, modelManager_);
	player_->SetMapMinMax(map_->GetMinX(), map_->GetMaxX(), map_->GetMinZ(), map_->GetMaxZ());

	objectRender_ = std::make_unique<ObjectRender>();
	objectRender_->Initialize(drawDataManager_, modelManager_);

	weaponDatabase_ = std::make_unique<WeaponDatabase>();
	weaponDatabase_->Initialize(jsonManager_);

	attackManager_ = std::make_unique<AttackManager>();
	attackManager_->Initialize(modelManager_);

	IWeapon::StaticInitialize(attackManager_.get(), enemyManager_.get(), weaponDatabase_.get());

	waveSystem_ = std::make_unique<WaveSystem>();
	waveSystem_->Initialize(enemyManager_.get(), commonData_->stageNum++, map_->GetMinX(), map_->GetMaxX(), map_->GetMinZ(), map_->GetMaxZ());

	gameTimer_ = std::make_unique<GameTimer>();
	gameTimer_->Initialize();

	aiController_ = std::make_unique<AIController>(player_->GetPositionPtr(), enemyManager_.get());
	inputController_ = std::make_unique<InputController>(input_);
	controllers_.push_back(aiController_.get());
	controllers_.push_back(inputController_.get());
	currentControllerIndex_ = 0;
	player_->SetController(controllers_[currentControllerIndex_]); // AIコントローラーを適用

	MakeWeapon();
}

std::unique_ptr<IScene> ShigeScene::Update() {

	float deltaTime = engine_->GetFPSObserver()->GetDeltatime();

	gameCamera_->Update(deltaTime, player_->GetTransform().position);
	Vector3 cameraPos = { 0.f, 0.f, 0.f };
	grid_->Update(cameraPos, camera_->GetVPMatrix());
	auto key = commonData_->keyManager->GetKeyStates();

	gameTimer_->Update(deltaTime);
	waveSystem_->Update(deltaTime);

	if(input_->GetKeyState(DIK_4)&&!input_->GetPreKeyState(DIK_4)){
		// インデックスを切り替える
		currentControllerIndex_ = (currentControllerIndex_ + 1) % controllers_.size();

		// プレイヤーに新しいコントローラーをセット
		player_->SetController(controllers_[currentControllerIndex_]);
	}

	player_->Update(camera_->GetVPMatrix(), deltaTime);
	player_->UpdateParameter(pieces_);
	map_->Update(camera_->GetVPMatrix());
	enemyManager_->Update(deltaTime);
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
				// 円周上の角度を計算 (ラジアン)
				float angle = (2.0f * std::numbers::pi_v<float> / weaponCount) * i;

				// XZ平面での円周オフセット座標の計算 (baseRadius_とbaseHeight_を使用)
				Vector3 offset = { std::cos(angle) * baseRadius_, baseHeight_, std::sin(angle) * baseRadius_ };

				// プレイヤー座標にオフセットを加算
				Vector3 weaponPos = player_->GetTransform().position;

				weaponRenders_[i]->Update(camera_->GetVPMatrix(), weaponPos);
			}
		}
	}

	if (key[Key::Debug1] || gameTimer_->IsEnd()) {
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
	map_->Draw(cmdObj);
	objectRender_->Draw(cmdObj);
	player_->Draw(cmdObj);

	for (const auto& render : weaponRenders_) {
		render->Draw(cmdObj);
	}

	display->PostDraw(cmdObj);

	window->PreDraw(cmdObj);

	// ここ以外で記述する場合、ifdefを忘れないようにすること
#ifdef USE_IMGUI

	display->DrawImGui();

	ImGui::Begin("FPS");
	float deltaTime = engine_->GetFPSObserver()->GetDeltatime();
	ImGui::Text("DeltaTime: %.3f ms", deltaTime * 1000.0f);
	ImGui::Text("FPS: %.1f", 1.0f / deltaTime);
	ImGui::End();

	ImGui::Begin("RenderDebug");
	ImGui::DragFloat("baseHeight", &baseHeight_, 0.01f);
	ImGui::DragFloat("baseRadius", &baseRadius_, 0.01f);
	ImGui::End();

	ImGui::Begin("ActiveController");
	ImGui::Text("%s", currentControllerIndex_ == 0 ? "AIController" : "InputController");
	ImGui::End();

#endif

	engine_->DrawImGui();
	window->PostDraw(cmdObj);
}

void ShigeScene::MakeWeapon() {
	for (const auto& piece : commonData_->pieces) {
		int weaponID = piece->GetItem().weaponID;

		if (weaponID != -1) {
			WeaponData* data = weaponDatabase_->GetWeapon(weaponID);
			std::unique_ptr<IWeapon> weapon;
			std::unique_ptr<IWeaponRender> weaponRender = std::make_unique<IWeaponRender>();

			switch (data->type) {
			case WeaponType::Pistol:
			{
				weapon = std::make_unique<Pistol>();
				break;
			}
			case WeaponType::Sword:
			{
				weapon = std::make_unique<Sword>();
				break;
			}
			case WeaponType::ShotGun:
			{
				weapon = std::make_unique<ShotGun>();
				break;
			}
			case WeaponType::Spear:
				weapon = std::make_unique<Spear>();
				break;
			}

			weaponRender->Initialize(drawDataManager_, modelManager_, weapon.get(), piece->GetItem());
			weaponRenders_.push_back(std::move(weaponRender));

			weapon->Initialize(weaponID, player_.get());
			weapons_.push_back(std::move(weapon));
		}
	}
}
