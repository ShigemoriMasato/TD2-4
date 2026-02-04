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

	cardManager_ = std::make_unique<CardManager>();
	DrawData drawData = drawDataManager_->GetDrawData(modelManager_->GetNodeModelData(1).drawDataIndex);
	cardManager_->Initialize(drawData, fontLoader_, textureManager_, commonData_->normaAndScore_);

	totalManager_ = std::make_unique<TotalManager>();
	totalManager_->Initialize(
		commonData_->maxOreNum,
		commonData_->getOreNum,
		commonData_->killOreNum,
		commonData_->sumGoldNum_,
		commonData_->maxGoldNum,
		drawData,
		fontLoader_);

	bgpe_ = std::make_unique<PostEffect>();
	bgpe_->Initialize(textureManager_, drawDataManager_->GetDrawData(commonData_->postEffectDrawDataIndex));
	bgConfig.jobs_ = uint32_t(PostEffectJob::HeavyBlur);
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

	miniMap_ = std::make_unique<MiniMap>();
	miniMap_->Initialize(
		static_cast<int>(prevMap_.mapChipData.size()),
		static_cast<int>(prevMap_.mapChipData[0].size()),
		commonData_->miniMapDisplay.get(),
		drawDataManager_->GetDrawData(modelManager_->GetNodeModelData(1).drawDataIndex),
		drawDataManager_->GetDrawData(modelManager_->GetNodeModelData(modelManager_->LoadModel("VisionFrame")).drawDataIndex),
		textureManager_);

	blur_.kernelSize = 5.0f;
	blur_.intensity = 1.0f;

	miniMap_->Update(true);
}

std::unique_ptr<IScene> ResultScene::Update() {
	float deltaTime = engine_->GetFPSObserver()->GetDeltatime();
	input_->Update();
	commonData_->keyManager->Update();
	auto key = commonData_->keyManager->GetKeyStates();


	//最初のリザルトが終わったらプレイヤーが切り替えられるようにする
	if (resultFirstFinished_) {
		if (key[Key::LeftTri]) {
			state = ResultState::Result;
		}

		if (key[Key::RightTri]) {
			state = ResultState::Total;
		}
	}

	CameraUpdate(deltaTime);

	if (cardManager_->Update(deltaTime) && !resultFirstFinished_) {
		state = ResultState::Total;
		resultFirstFinished_ = true;
	}

	totalManager_->Update(deltaTime);

	if(resultFirstFinished_ && key[Key::Correct]) {
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
		MapInit_ = true;
		Matrix4x4 vpMat = miniMap_->PreDraw(window)->GetVPMatrix();
		mapRender_->Draw(vpMat, window);
		staticObjectRender_->Draw(vpMat, window);
		miniMap_->PostDraw(window, {}, {}, 0.0f);
	}

	bgpe_->CopyBuffer(PostEffectJob::HeavyBlur, blur_);
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
