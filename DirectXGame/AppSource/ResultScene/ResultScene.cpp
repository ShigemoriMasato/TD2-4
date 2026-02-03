#include "ResultScene.h"
#include <Utility/Easing.h>

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
		{10, 60}
	};

	cardManager_ = std::make_unique<CardManager>();
	DrawData drawData = drawDataManager_->GetDrawData(modelManager_->GetNodeModelData(1).drawDataIndex);
	cardManager_->Initialize(drawData, fontLoader_, textureManager_, commonData_->stageNorma_);
}

std::unique_ptr<IScene> ResultScene::Update() {
	float deltaTime = engine_->GetFPSObserver()->GetDeltatime();
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

	return nullptr;
}

void ResultScene::Draw() {
	auto gWindow = commonData_->mainWindow.get();
	auto window = gWindow->GetWindow();
	auto display = commonData_->display.get();

	display->PreDraw(window->GetCommandObject(), true);

	cardManager_->Draw(window, camera_->GetVPMatrix());

	display->PostDraw(window->GetCommandObject());

	gWindow->PreDraw(true);

#ifdef USE_IMGUI


	gWindow->DrawDisplayWithImGui();
	engine_->ImGuiDraw();

#endif
}

void ResultScene::CameraUpdate(float deltaTime) {
	switch (state) {
	case ResultState::Result:
		cameraPos_ = lerp(cameraPos_, Vector3(-740.0f, 0.0f, 0.0f), deltaTime * 2.0f);
		break;
	case ResultState::Total:
		cameraPos_ = lerp(cameraPos_, Vector3(640.0f, 0.0f, 0.0f), deltaTime * 2.0f);
		break;
	}
	camera_->SetTransform(Matrix::MakeTranslationMatrix(cameraPos_).Inverse());
	camera_->MakeMatrix();
}
