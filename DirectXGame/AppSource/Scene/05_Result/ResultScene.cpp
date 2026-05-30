#include "ResultScene.h"
#include "GameObject/Random/Random.h"
#include <02_ShigeScene/ShigeScene.h>
#include <Scene/01_Title/TitleScene.h>
#include <Utility/Color.h>
#include <format>
#include <imgui/imgui.h>

using namespace SHEngine;

ResultScene::ResultScene() {}

void ResultScene::Initialize() {
	camera_ = std::make_unique<Camera>();
	PerspectiveFovDesc perspectiveDesc{};
	camera_->SetProjectionMatrix(perspectiveDesc);
	camera_->SetPosition({0.0f, 0.0f, -35.0f});
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

	orthoCamera_ = std::make_unique<Camera>();
	orthoCamera_->SetProjectionMatrix(OrthographicDesc{});

	retryText_ = std::make_unique<Text>(64);
	retryText_->Initialize(textDrawData, "YDWbananaslipplus.otf", 64);
	retryText_->SetText(L"リトライ");
	retryText_->SetColor({1.0f, 1.0f, 1.0f, 1.0f});
	retryText_->SetSize(2.0f);
	retryTextTransform_.position = {240.0f, -630.0f, 0.0f};

	toTitleText_ = std::make_unique<Text>(64);
	toTitleText_->Initialize(textDrawData, "YDWbananaslipplus.otf", 64);
	toTitleText_->SetText(L"タイトルへ");
	toTitleText_->SetColor({1.0f, 1.0f, 1.0f, 1.0f});
	toTitleText_->SetSize(2.0f);
	toTitleTextTransform_.position = {720.0f, -630.0f, 0.0f};

	uiManager_ = std::make_unique<ResultUIManager>();
	uiManager_->Initialize(textDrawData, commonData_->killCount, commonData_->clearTime, commonData_->keyManager.get());

	isWin_ = commonData_->isWin;

	postEffect_ = std::make_unique<PostEffect>();
	postEffect_->Initialize(textureManager_, drawDataManager_->GetDrawData(commonData_->postEffectDrawDataIndex));
	postEffectConfig_.cmdObj = commonData_->cmdObject.get();
	postEffectConfig_.origin = commonData_->display->GetDisplay();
	postEffectConfig_.jobs_ = static_cast<uint32_t>(PostEffectJob::Bloom);

	posAnime_.anim.Start(100.0f, -230.0f, 2.0f, EaseType::EaseOutBounce);

	sword_ = std::make_unique<ResultSword>();
	sword_->Initialize(modelManager_, drawDataManager_, textureManager_);

	fadeManager_ = std::make_unique<FadeManager>();
	fadeManager_->Initialize(modelManager_, drawDataManager_);
	fadeManager_->StartFadeOut(false);

	enemyRainManager_ = std::make_unique<EnemyRainManager>();
	enemyRainManager_->Initilaize(modelManager_, drawDataManager_);

	dirLight_.color = {1.0f, 1.0f, 1.0f, 1.0f};
	dirLight_.direction = {0.0f, 1.0f, 0.0f};
	dirLight_.intensity = 2.0f;

	SHEngine::DrawData planeDrawData = drawDataManager_->GetDrawData(modelManager_->GetNodeModelData(1).drawDataIndex);
	gameFrame_ = std::make_unique<GameFrame>();
	gameFrame_->Initialize(planeDrawData, textureManager_->LoadTexture("TitleFrame.png"));

	bloom_.intensity = 0.15f;
	bloom_.radius = 0.8f;
	bloom_.softness = 1.0f;
	bloom_.color = {0.0f, 0.0f, 1.0f, 1.0f};

	sparkParticle_ = std::make_unique<MultiParticle>();
	sparkParticle_->Initialize(textureManager_, modelManager_, commonData_);
	sparkParticle_->Add("spark2");
	sparkParticle_->Add("spark3");
	sparkParticle_->SetEmittingFlag(false);
	sparkParticleModelWorld_ = Matrix4x4::Identity();
}

std::unique_ptr<IScene> ResultScene::Update() {
	float deltaTime = engine_->GetFPSObserver()->GetDeltatime();
	auto key = commonData_->keyManager->GetKeyStates();

	{
		orthoCamera_->SetScale({1, -1, 1});
		orthoCamera_->SetPosition({0, 0, 0});
		orthoCamera_->MakeMatrix();
	}

	bool playing = posAnime_.anim.Update(engine_->GetDeltaTime(), posAnime_.temp);
	clearTextTransform_.position.y = posAnime_.temp;
	gameOverTextTransform_.position.y = posAnime_.temp;

	if (!playing) {
		uiManager_->StartAnimation();
		pendingTime_ += engine_->GetDeltaTime();

		if (pendingTime_ >= 0.5f) {
			posYTime_ += engine_->GetDeltaTime();
			float t = (std::sinf(posYTime_) * 0.5f);
			float value = t * 0.5f;

			clearTextTransform_.position.y -= value * 100.0f;
			gameOverTextTransform_.position.y -= value * 100.0f;
		}
	}

	uiManager_->Update(orthoCamera_->GetVPMatrix(), deltaTime);

	clearText_->SetTransform(clearTextTransform_);
	clearText_->Update(orthoCamera_->GetVPMatrix());

	gameOverText_->SetTransform(gameOverTextTransform_);
	gameOverText_->Update(orthoCamera_->GetVPMatrix());

	sword_->Update(camera_->GetVPMatrix(), deltaTime);
	enemyRainManager_->Update(camera_->GetVPMatrix(), deltaTime, dirLight_);

	retryText_->SetTransform(retryTextTransform_);
	retryText_->Update(orthoCamera_->GetVPMatrix());

	toTitleText_->SetTransform(toTitleTextTransform_);
	toTitleText_->Update(orthoCamera_->GetVPMatrix());

	if (isDeciding_) {
		bool playing = decideScaleAnime_.anim.Update(deltaTime, decideScaleAnime_.temp);

		if (selectedIndex_ == 0) {
			retryText_->SetSize(decideScaleAnime_.temp);
		} else {
			toTitleText_->SetSize(decideScaleAnime_.temp);
		}
	}

	fadeManager_->Update(camera_->GetVPMatrix(), deltaTime);

	gameFrame_->Update();

	if (auto next = fadeManager_->TakeNextScene()) {
		return next;
	}

	if (key[Key::Retry]) {
		selectedIndex_ = (selectedIndex_ - 1 + 2) % 2;
	}
	if (key[Key::ToTitle]) {
		selectedIndex_ = (selectedIndex_ + 1) % 2;
	}
	if (key[Key::Correct] && !isDeciding_) {
		isDeciding_ = true;
		float startSize = 2.0f;
		decideScaleAnime_.anim.Start(startSize, startSize * 1.5f, 0.3f, EaseType::EaseOutBack);
		sword_->StartAnimation();
	}

	if (sword_->IsAnimationFinished() && !isPreFinished_) {
		if (!isCameraShaking_) {
			// Particle開始
			if (mrSecond)sparkParticle_->SetEmittingFlag(true);
			mrSecond = true;

			// カメラシェイク開始
			isCameraShaking_ = true;
			shakeTime_ = 0.0f;
			cameraBasePos_ = camera_->GetPosition();
			orthoCameraBasePos_ = orthoCamera_->GetPosition();

			// フェード開始
			if (selectedIndex_ == 0) {
				fadeManager_->StartFadeIn([]() { return std::make_unique<ShigeScene>(); });
			} else {
				fadeManager_->StartFadeIn([]() { return std::make_unique<TitleScene>(); });
			}
		}
	}

	isPreFinished_ = sword_->IsAnimationFinished();

	if (isCameraShaking_) {
		shakeTime_ += deltaTime;

		if (shakeTime_ >= shakeDuration_) {
			isCameraShaking_ = false;
			camera_->SetPosition(cameraBasePos_);
		} else {
			float t = 1.0f - (shakeTime_ / shakeDuration_);
			float offsetX = (RandomUtils::RangeFloat(-1.0f, 1.0f) * shakeIntensity_) * t;
			float offsetY = (RandomUtils::RangeFloat(-1.0f, 1.0f) * shakeIntensity_) * t;

			camera_->SetPosition({cameraBasePos_.x + offsetX, cameraBasePos_.y + offsetY, cameraBasePos_.z});
		}

		camera_->MakeMatrix();
	}

	UpdateSelectVisual();

	Matrix4x4 test = Matrix::MakeAffineMatrix(
		sparkParticleTransform_.scale,
		sparkParticleTransform_.rotate,
		sparkParticleTransform_.position);
	sparkParticle_->SetModelWorld(test);
	sparkParticle_->SetCameraPos(camera_->GetPosition());
	sparkParticle_->Update(deltaTime);

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

	if (ImGui::TreeNode("Spark Particle"))
	{
		ImGui::DragFloat3("Position", &sparkParticleTransform_.position.x, 0.1f);
		ImGui::DragFloat3("Rotation", &sparkParticleTransform_.rotate.x, 0.01f);
		ImGui::DragFloat3("Scale", &sparkParticleTransform_.scale.x, 0.01f, 0.01f, 10.0f);

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

	uiManager_->Draw(cmdObj);
	retryText_->Draw(cmdObj);
	toTitleText_->Draw(cmdObj);

	sparkParticle_->Draw();
	commonData_->particleDrawer->Draw(cmdObj, camera_->GetVPMatrix());

	enemyRainManager_->Draw(cmdObj);
	sword_->Draw(cmdObj);

	fadeManager_->Draw(cmdObj);
	gameFrame_->Draw(cmdObj);


	display->PostDraw(cmdObj);

#ifdef USE_IMGUI
	postEffect_->Draw(postEffectConfig_);
	window->PreDraw(cmdObj, true);
#else
	postEffectConfig_.output = commonData_->mainWindow.second->GetCurrentDisplay();
	postEffect_->Draw(postEffectConfig_);
	window->PreDraw(cmdObj, false);
#endif

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

	ImGui::DragFloat2("Retry Text Position", &retryTextTransform_.position.x, 1.0f);
	ImGui::DragFloat2("ToTitle Text Position", &toTitleTextTransform_.position.x, 1.0f);
	ImGui::End();

	ImGui::Begin("DirectionalLight");
	ImGui::DragFloat3("direction", &dirLight_.direction.x, 0.01f);
	ImGui::DragFloat("intensity", &dirLight_.intensity, 0.1f);
	ImGui::ColorEdit4("Color", &dirLight_.color.x);
	ImGui::End();

	ImGui::Begin("PostEffect");
	ImGui::DragFloat("intensity", &bloom_.intensity, 0.01f);
	ImGui::DragFloat("radius", &bloom_.radius, 0.01f);
	ImGui::DragFloat("softness", &bloom_.softness, 0.01f);
	ImGui::ColorEdit4("color", &bloom_.color.x);
	ImGui::End();

	dirLight_.direction = dirLight_.direction.Normalize();

#endif
	postEffect_->CopyBuffer(PostEffectJob::Bloom, bloom_);

	engine_->DrawImGui();
	window->ToPresent(cmdObj);
}

void ResultScene::UpdateSelectVisual() {
	if (isDeciding_)
		return;

	if (selectedIndex_ == 0) {
		retryText_->SetColor({1, 0, 0, 1});
		retryText_->SetSize(3.0f);

		toTitleText_->SetColor({1, 1, 1, 1});
		toTitleText_->SetSize(2.0f);
	} else {
		retryText_->SetColor({1, 1, 1, 1});
		retryText_->SetSize(2.0f);

		toTitleText_->SetColor({1, 0, 0, 1});
		toTitleText_->SetSize(3.0f);
	}
}
