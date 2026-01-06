#include "GameScene.h"
#include <imgui/imgui.h>
#include <Utility/Color.h>

namespace {
}

GameScene::GameScene() {
	tetris_ = std::make_unique<Tetris>();
	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize();
	manualCamera_ = std::make_unique<Camera>();
	manualCamera_->SetProjectionMatrix(PerspectiveFovDesc());
	manualCamera_->MakeMatrix();
	postEffect_ = std::make_unique<PostEffect>();
	gameCamera_ = std::make_unique<GameCamera>();

	backGround_ = std::make_unique<BackGround>();
	gameMonitor_ = std::make_unique<RenderObject>("GameMonitor");
	fallLight_ = std::make_unique<FallLight>();

	gameCamera_->Initialize();
	worldCamera_ = gameCamera_->GetCamera();
}

void GameScene::Initialize() {
	keyCoating_ = std::make_unique<KeyCoating>(commonData_->keyManager.get());
	auto model = modelManager_->GetNodeModelData(0);	//Cube
	DrawData drawData = drawDataManager_->GetDrawData(model.drawDataIndex);

	tetris_->Initialize(keyCoating_.get(), worldCamera_, drawData);
	backGround_->Initialize(drawData, worldCamera_);

	model = modelManager_->GetNodeModelData(1);	//Plane
	drawData = drawDataManager_->GetDrawData(model.drawDataIndex);
	fallLight_->Initialize(drawData, textureManager_, worldCamera_);

	drawData = drawDataManager_->GetDrawData(commonData_->postEffectDrawDataIndex);
	postEffect_->Initialize(textureManager_, drawData);
	gameMonitor_->Initialize();
	gameMonitor_->SetDrawData(drawData);
	gameMonitor_->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "GameMonitor.TextureIndex");
	gameMonitor_->SetUseTexture(true);
	gameMonitor_->psoConfig_.depthStencilID = DepthStencilID::Transparent;
	gameMonitor_->psoConfig_.vs = "PostEffect/PostEffect.VS.hlsl";
	gameMonitor_->psoConfig_.ps = "PostEffect/Simple.PS.hlsl";
	gameMonitor_->psoConfig_.isSwapChain = true;

	postEffectConfig_.jobs_ = PostEffectJob::Blur | PostEffectJob::SlowMotion;
}

std::unique_ptr<IScene> GameScene::Update() {
	float deltaTime = engine_->GetFPSObserver()->GetDeltatime();

	int offset = commonData_->display->GetTextureData()->GetOffset();
	gameMonitor_->CopyBufferData(0, &offset, sizeof(int));

	input_->Update();
	commonData_->keyManager->Update();
	keyCoating_->Update(deltaTime);

	debugCamera_->Update();
	gameCamera_->Update(deltaTime);

	tetris_->Update(deltaTime);

	fallLight_->Update(deltaTime);

	//線を消したときのやつ
	int deleteNum = tetris_->IsLineDeleted();
	if (deleteNum) {
		auto wave = backGround_->GetWave();
		wave.strength = 2.0f * deleteNum;
		wave.color = ConvertColor(0x9f33Bfff);
		backGround_->AddWave(wave);

		if (deleteNum > 2) {
			gameCamera_->Shake(0.3f * deleteNum, 0.5f);
		}
	}
	auto key = keyCoating_->GetKeyStates();

	if (key.at(Key::Left)) {
		auto wave = backGround_->GetWave();
		wave.strength = 1.0f;
		wave.generatePosition.x = -18.0f;
		wave.lifeSpan = 0.5f;
		backGround_->AddWave(wave);
	}

	if (key.at(Key::Right)) {
		auto wave = backGround_->GetWave();
		wave.strength = 1.0f;
		wave.generatePosition.x = 18.0f;
		wave.lifeSpan = 0.5f;
		backGround_->AddWave(wave);
	}

	if (key.at(Key::Down)) {
		auto wave = backGround_->GetWave();
		wave.strength = 1.0f;
		wave.generatePosition.y = -9.0f;
		wave.lifeSpan = 0.5f;
		backGround_->AddWave(wave);
	}

	if (key.at(Key::HardDrop)) {
		auto wave = backGround_->GetWave();
		wave.strength = 1.0f;
		wave.generatePosition.y = 27.0f;
		wave.generatePosition.x = 4.0f;
		wave.lifeSpan = 0.5f;
		backGround_->AddWave(wave);
		wave.generatePosition.x = -4.0f;
		wave.lifeSpan = 0.5f;
		backGround_->AddWave(wave);
	}


	backGround_->Update(engine_->GetFPSObserver()->GetDeltatime());

	if (keyCoating_->GetKeyStates().at(Key::Debug1)) {
		return std::make_unique<GameScene>();
	}

	return nullptr;
}

void GameScene::Draw() {
	auto window = commonData_->mainWindow.get();
	auto display = commonData_->display.get();

	display->PreDraw(window->GetCommandList(), true);
	backGround_->Draw(window->GetWindow());
	fallLight_->Draw(window->GetWindow());
	tetris_->Draw(window->GetWindow());
	display->PostDraw(window->GetCommandList());

	window->PreDraw();
	gameMonitor_->Draw(window->GetWindow());
	window->PostDraw();

	//ここ以外で記述する場合、ifdefを忘れないようにすること
#ifdef USE_IMGUI
	manualCamera_->DrawImGui();
	manualCamera_->MakeMatrix();
	tetris_->DrawImGui();
	backGround_->DrawImGui();
	gameCamera_->DrawImGui();
	fallLight_->DrawImGui();
#endif

	engine_->ImGuiDraw();
}
