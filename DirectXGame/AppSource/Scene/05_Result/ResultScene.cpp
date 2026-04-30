#include "ResultScene.h"
#include <Scene/01_Title/TitleScene.h>
#include <Utility/Color.h>
#include <imgui/imgui.h>

using namespace SHEngine;

ResultScene::ResultScene() {}

void ResultScene::Initialize() {
	camera_ = std::make_unique<Camera>();
	PerspectiveFovDesc perspectiveDesc{};
	camera_->SetProjectionMatrix(perspectiveDesc);
	camera_->SetPosition({0.0f, 0.0f, -20.0f});
	camera_->SetRotation({0.0f, 0.0f, 0.0f});
	camera_->SetScale({1.0f, 1.0f, 1.0f});
	camera_->MakeMatrix();

	// テキストの初期化
	int planeModelHandle = modelManager_->LoadModel("Assets/.EngineResource/Model/Plane");
	auto planeModelData = modelManager_->GetNodeModelData(planeModelHandle);
	SHEngine::DrawData textDrawData = drawDataManager_->GetDrawData(planeModelData.drawDataIndex);

	clearText_ = std::make_unique<Text>();
	clearText_->Initialize(textDrawData, "YDWbananaslipplus.otf", 64);
	clearText_->SetText(L"ゲームクリア");
	clearText_->SetColor({1.0f, 1.0f, 0.0f, 1.0f});
	clearText_->SetSize(2.0f);
	clearTextTransform_.position = {486.0f, 100.0f, 0.0f};

	gameOverText_ = std::make_unique<Text>();
	gameOverText_->Initialize(textDrawData, "YDWbananaslipplus.otf", 64);
	gameOverText_->SetText(L"ゲームオーバー");
	gameOverText_->SetColor({1.0f, 0.0f, 0.0f, 1.0f});
	gameOverText_->SetSize(2.0f);
	gameOverTextTransform_.position = {455.0f, 100.0f, 0.0f};

	CorrectText_ = std::make_unique<Text>();
	CorrectText_->Initialize(textDrawData, "YDWbananaslipplus.otf", 64);
	CorrectText_->SetText(L"--- Zキー　か　スペースキーで　タイトルへ ---");
	CorrectText_->SetColor({1.0f, 1.0f, 1.0f, 1.0f});
	CorrectText_->SetSize(2.0f);
	correctTextTransform_.position = {45.0f, -500.0f, 0.0f};

	orthoCamera_ = std::make_unique<Camera>();
	orthoCamera_->SetProjectionMatrix(OrthographicDesc{});

	isWin_ = commonData_->isWin;

	postEffect_ = std::make_unique<PostEffect>();
	postEffect_->Initialize(textureManager_, drawDataManager_->GetDrawData(commonData_->postEffectDrawDataIndex));
	postEffectConfig_.cmdObj = commonData_->cmdObject.get();
	postEffectConfig_.origin = commonData_->display->GetDisplay();

	posAnime_.anim.Start(100.0f, -250.0f, 1.0f, EaseType::EaseOutBounce);

	sword_ = std::make_unique<ResultSword>();
	sword_->Initialize(modelManager_, drawDataManager_, textureManager_);
}

std::unique_ptr<IScene> ResultScene::Update() {
	float deltaTime = engine_->GetFPSObserver()->GetDeltatime();

	{
		orthoCamera_->SetScale({1, -1, 1});
		orthoCamera_->SetPosition({0, 0, 0});
		orthoCamera_->MakeMatrix();
	}

	bool playing = posAnime_.anim.Update(engine_->GetDeltaTime(), posAnime_.temp);
	clearTextTransform_.position.y = posAnime_.temp;
	gameOverTextTransform_.position.y = posAnime_.temp;

	if (!playing) {
		pendingTime_ += engine_->GetDeltaTime();

		if (pendingTime_ >= 0.5f) {
			posYTime_ += engine_->GetDeltaTime();
			float t = (std::sinf(posYTime_) * 0.5f);
			float value = t * 0.5f;

			clearTextTransform_.position.y -= value * 100.0f;
			gameOverTextTransform_.position.y -= value * 100.0f;
		}
	}

	clearText_->SetTransform(clearTextTransform_);
	clearText_->Update(orthoCamera_->GetVPMatrix());

	gameOverText_->SetTransform(gameOverTextTransform_);
	gameOverText_->Update(orthoCamera_->GetVPMatrix());

	CorrectText_->SetTransform(correctTextTransform_);
	alphaTime_ += engine_->GetDeltaTime();
	float t = (std::sinf(alphaTime_) * 0.5f) + 0.5f;
	float value = 0.1f + t * 0.5f;
	CorrectText_->SetColor({1.0f, 1.0f, 1.0f, value});
	CorrectText_->Update(orthoCamera_->GetVPMatrix());

	sword_->Update(camera_->GetVPMatrix(), deltaTime);

	auto key = commonData_->keyManager->GetKeyStates();
	if (key[Key::Correct]) {
		return std::make_unique<TitleScene>();
	}

	if (key[Key::Debug1]) {
		sword_->StartAnimation();
	}

#ifdef USE_IMGUI
	ImGui::Begin("Result Scene Settings");

	// カメラの設定
	if (ImGui::TreeNode("Camera")) {
		static Vector3 cameraPos = {0.0f, 0.0f, -20.0f};
		static Vector3 cameraRot = {0.0f, 0.0f, 0.0f};
		static Vector3 cameraScale = {1.0f, 1.0f, 1.0f};

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

	ImGui::End();
#endif

	return nullptr;
}

void ResultScene::Draw() {
	auto window = commonData_->mainWindow.second.get();
	auto display = commonData_->display.get();
	auto cmdObj = commonData_->cmdObject.get();

	display->PreDraw(cmdObj, true);

	if (isWin_) {
		clearText_->Draw(cmdObj);
	} else {
		gameOverText_->Draw(cmdObj);
	}

	CorrectText_->Draw(cmdObj);

	sword_->Draw(cmdObj);

	display->PostDraw(cmdObj);

#ifdef SH_RELEASE
	postEffectConfig_.output = commonData_->mainWindow.second->GetCurrentDisplay();
	postEffect_->Draw(postEffectConfig_);
#endif

	window->PreDraw(cmdObj, false);

	// ここ以外で記述する場合、ifdefを忘れないようにすること
#ifdef USE_IMGUI

	display->DrawImGui();

	ImGui::Begin("FPS");
	float deltaTime = engine_->GetFPSObserver()->GetDeltatime();
	ImGui::Text("DeltaTime: %.3f ms", deltaTime * 1000.0f);
	ImGui::Text("FPS: %.1f", 1.0f / deltaTime);
	ImGui::End();

	ImGui::Begin("Text");
	ImGui::DragFloat2("GameClear Text Position", &clearTextTransform_.position.x, 1.0f);
	ImGui::DragFloat2("GameOver Text Position", &gameOverTextTransform_.position.x, 1.0f);
	ImGui::DragFloat2("Correct Text Position", &correctTextTransform_.position.x, 1.0f);
	ImGui::End();
#endif

	engine_->DrawImGui();
	window->ToPresent(cmdObj);
}
