#include "ResultScene.h"
#include <Utility/Easing.h>
#include <Game/SelectScene.h>

namespace {
	std::string fontName = "YDWbananaslipplus.otf";
}

void ResultScene::Initialize() {
	camera_ = std::make_unique<Camera>();
	camera_->SetProjectionMatrix(OrthographicDesc());
	camera_->SetTransform(Matrix::MakeTranslationMatrix({ -740.0f, 0.0f, 0.0f }).Inverse());
	camera_->MakeMatrix();

	commonData_->stageNorma_ = {
		{20, 10},
		{40, 20},
		{60, 30},
		{80, 40},
		{100, 50},
		{70, 60},
		{ 20, 10 },
		{40, 20},
		{20, 30},
	};

	cardManager_ = std::make_unique<CardManager>();
	DrawData drawData = drawDataManager_->GetDrawData(modelManager_->GetNodeModelData(1).drawDataIndex);
	cardManager_->Initialize(drawData, fontLoader_, textureManager_, commonData_->stageNorma_);

	totalManager_ = std::make_unique<TotalManager>();
	totalManager_->Initialize(
		commonData_->maxOreNum,
		commonData_->killOreNum,
		commonData_->maxGoldNum,
		commonData_->currentGoldNum_,
		commonData_->maxGoldNum,
		drawData,
		fontLoader_);

	bgpe_ = std::make_unique<PostEffect>();
	bgpe_->Initialize(textureManager_, drawDataManager_->GetDrawData(commonData_->postEffectDrawDataIndex));
	bgConfig.jobs_ = uint32_t(PostEffectJob::Blur);
	bgConfig.origin = commonData_->miniMapDisplay.get();
	bgConfig.window = commonData_->mainWindow->GetWindow();
	bgConfig.output = commonData_->display.get();

	pe_ = std::make_unique<PostEffect>();
	pe_->Initialize(textureManager_, drawDataManager_->GetDrawData(commonData_->postEffectDrawDataIndex));
	config.jobs_ = uint32_t(PostEffectJob::Fade);
	config.origin = commonData_->display.get();
	config.window = commonData_->mainWindow->GetWindow();
#ifdef USE_IMGUI
	config.output = commonData_->display.get();
#endif

	prevMap_ = commonData_->newMapManager->GetMapData(commonData_->prevMapIndex);
	mapRender_ = std::make_unique<MapRender>();
	DrawData box = drawDataManager_->GetDrawData(modelManager_->GetNodeModelData(0).drawDataIndex);
	mapRender_->Initialize(box, 0);
	mapRender_->SetConfig(prevMap_.renderData);

	staticObjectRender_ = std::make_unique<StaticObjectRender>();
	staticObjectRender_->Initialize(modelManager_, drawDataManager_, false);
	staticObjectRender_->SetObjects(prevMap_.decorations);
}

std::unique_ptr<IScene> ResultScene::Update() {
	float deltaTime = engine_->GetFPSObserver()->GetDeltatime();
	input_->Update();
	commonData_->keyManager->Update();
	auto key = commonData_->keyManager->GetKeyStates();

	//最初のリザルトが終わったらプレイヤーが切り替えられるようにする
	if (reusltFirstFinished_) {
		if (key[Key::LeftTri]) {
			state = ResultState::Result;
		}

		if (key[Key::RightTri]) {
			state = ResultState::Total;
		}
	}

	CameraUpdate(deltaTime);

	if (cardManager_->Update(deltaTime) && !reusltFirstFinished_) {
		state = ResultState::Total;
		reusltFirstFinished_ = true;
	}

	totalManager_->Update(deltaTime);

	if(reusltFirstFinished_ && key[Key::Correct]) {
		fadeOut_ = true;
	}

	if (fadeOut_) {
		fade_.alpha += deltaTime;
		if (fade_.alpha >= 1.0f) {
			static float time = 0.0f;
			time += deltaTime;
			if (time > 0.5f) {
				time = 0.0f;
				return std::make_unique<SelectScene>();
			}
		}
	}

	return nullptr;
}

void ResultScene::Draw() {
	auto gWindow = commonData_->mainWindow.get();
	auto window = gWindow->GetWindow();
	auto display = commonData_->display.get();

	if (!MapInit_) {

	}

	bgpe_->CopyBuffer(PostEffectJob::Blur, blur_);
	bgpe_->Draw(bgConfig);

	display->PreDraw(window->GetCommandObject(), false);

	cardManager_->Draw(window, camera_->GetVPMatrix());
	totalManager_->Draw(window, camera_->GetVPMatrix());

	display->PostDraw(window->GetCommandObject());
	
#ifdef SH_RELEASE
	config.output = window->GetDisplay();
#endif

	pe_->CopyBuffer(PostEffectJob::Fade, fade_);
	pe_->Draw(config);

	gWindow->PreDraw(true);

#ifdef USE_IMGUI

	ImGui::Begin("Blur");
	ImGui::DragFloat("Intensity", &blur_.intensity, 0.01f, 0.0f, 5.0f);
	ImGui::DragFloat("Size", &blur_.kernelSize, 0.1f, 0.0f, 20.0f);
	ImGui::End();

	totalManager_->DrawImGui();

	gWindow->DrawDisplayWithImGui();
	engine_->ImGuiDraw();

#endif
}

void ResultScene::CameraUpdate(float deltaTime) {
	switch (state) {
	case ResultState::Result:
		cameraPos_ = lerp(cameraPos_, Vector3(-740.0f, 0.0f, 0.0f), deltaTime * 5.0f);
		break;
	case ResultState::Total:
		cameraPos_ = lerp(cameraPos_, Vector3(640.0f, 0.0f, 0.0f), deltaTime * 5.0f);
		break;
	}
	camera_->SetTransform(Matrix::MakeTranslationMatrix(cameraPos_).Inverse());
	camera_->MakeMatrix();
}
