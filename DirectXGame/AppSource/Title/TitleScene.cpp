#include "TitleScene.h"
#include <Game/GameScene.h>

TitleScene::TitleScene() {
	gameMonitor_ = std::make_unique<RenderObject>("TitleScene::GameMonitor");
	waterWave_ = std::make_unique<WaterWave>();
	camera_ = std::make_unique<Camera>();
	title_ = std::make_unique<Title>();
}

TitleScene::~TitleScene() {
}

void TitleScene::Initialize() {
	int modelIndex = modelManager_->LoadModel("WaterPlane");
	auto model = modelManager_->GetNodeModelData(modelIndex);
	auto drawData = drawDataManager_->GetDrawData(model.drawDataIndex);
	waterWave_->Initialize(drawData, camera_.get());

	modelIndex = modelManager_->LoadModel("Title");
	model = modelManager_->GetNodeModelData(modelIndex);
	drawData = drawDataManager_->GetDrawData(model.drawDataIndex);
	title_->Initialize(drawData, camera_.get());

	drawData = drawDataManager_->GetDrawData(commonData_->postEffectDrawDataIndex);
	gameMonitor_->Initialize();
	gameMonitor_->SetDrawData(drawData);
	gameMonitor_->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "GameMonitor.TextureIndex");
	gameMonitor_->SetUseTexture(true);
	gameMonitor_->psoConfig_.depthStencilID = DepthStencilID::Transparent;
	gameMonitor_->psoConfig_.vs = "PostEffect/PostEffect.VS.hlsl";
	gameMonitor_->psoConfig_.ps = "PostEffect/Simple.PS.hlsl";
	gameMonitor_->psoConfig_.isSwapChain = true;

	camera_->SetProjectionMatrix(PerspectiveFovDesc());
	camera_->MakeMatrix();
}

std::unique_ptr<IScene> TitleScene::Update() {
	float deltaTime = engine_->GetFPSObserver()->GetDeltatime();
	input_->Update();
	commonData_->keyManager->Update();

	camera_->MakeMatrix();

	int gameDisplayOffset = commonData_->display->GetTextureData()->GetOffset();
	gameMonitor_->CopyBufferData(0, &gameDisplayOffset, sizeof(int));
	waterWave_->Update(deltaTime);
	title_->Update(deltaTime);

	auto key = commonData_->keyManager->GetKeyStates();

	if (key.at(Key::Hold) || key.at(Key::HardDrop)) {
		return std::make_unique<GameScene>();
	}

	return nullptr;
}

void TitleScene::Draw() {
	auto window = commonData_->mainWindow.get();
	auto display = commonData_->display.get();

	display->PreDraw(window->GetCommandList(), true);
	waterWave_->Draw(window->GetWindow());
	title_->Draw(window->GetWindow());
	display->PostDraw(window->GetCommandList());

	window->PreDraw();
	gameMonitor_->Draw(window->GetWindow());
	window->PostDraw();

	//ここ以外で記述する場合、ifdefを忘れないようにすること
#ifdef USE_IMGUI
	title_->DrawImGui();
	waterWave_->DrawImGui();
	camera_->DrawImGui();
#endif

	engine_->ImGuiDraw();
}
