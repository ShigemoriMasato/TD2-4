#include "TitleScene.h"
#include <imgui/imgui.h>
#include <Utility/Color.h>

#include <02_ShigeScene/ShigeScene.h>

#include <../Engine/Assets/Audio/AudioManager.h>

TitleScene::TitleScene() {
}

TitleScene::~TitleScene() {
	bgm_->Stop();
}

void TitleScene::Initialize() {
	// CommonDataの音量設定へのポインタを取得
	masterVolume_ = &commonData_->masterVolume;
	bgmVolume_ = &commonData_->bgmVolume;
	seVolume_ = &commonData_->seVolume;

	// 音量を計算
	UpdateCalculatedVolumes();

	titleUI_ = std::make_unique<TitleUI>();
	titleUI_->Initialize(drawDataManager_, modelManager_, commonData_);

	camera_ = std::make_unique<Camera>();
	PerspectiveFovDesc perspectiveDesc;
	perspectiveDesc.SetValue(1280, 720, 0.45f, 0.1f, 1000.0f);
	camera_->SetProjectionMatrix(perspectiveDesc);
	camera_->SetPosition({ 0.0f, 0.0f, 0.0f });
	camera_->SetRotation({ 0.0f, 0.0f, 0.0f });
	camera_->SetScale({ 1.0f, 1.0f, 1.0f });

	bgm_ = AudioManager::GetInstance()->GetData("TitleScene.mp3")->CustomPlay(255);

	postEffect_ = std::make_unique<PostEffect>();
	postEffect_->Initialize(textureManager_, drawDataManager_->GetDrawData(commonData_->postEffectDrawDataIndex));
	postEffectConfig_.cmdObj = commonData_->cmdObject.get();
	postEffectConfig_.origin = commonData_->display->GetDisplay();
}

std::unique_ptr<IScene> TitleScene::Update() {
	auto keys = commonData_->keyManager->GetKeyStates();

	if (keys[Key::Correct]) {
		return std::make_unique<ShigeScene>();
	}

	return nullptr;
}

void TitleScene::Draw() {
	auto window = commonData_->mainWindow.second.get();
	auto display = commonData_->display.get();
	auto cmdObj = commonData_->cmdObject.get();

	// ディスプレイへの描画開始
	display->PreDraw(cmdObj, true);

	// TitleUIの描画（displayに描画）
	titleUI_->Draw(cmdObj);

	// ディスプレイへの描画終了
	display->PostDraw(cmdObj);

#ifdef SH_RELEASE
	postEffectConfig_.output = commonData_->mainWindow.second->GetCurrentDisplay();
	postEffect_->Draw(postEffectConfig_);

	// ウィンドウへの描画（displayの内容を転送）
	window->PreDraw(cmdObj, false);

#else

	// ウィンドウへの描画（displayの内容を転送）
	window->PreDraw(cmdObj, true);

#endif

	//ここ以外で記述する場合、ifdefを忘れないようにすること
#ifdef USE_IMGUI

	ImGui::Begin("Title Scene Settings");

	// カメラの設定
	if (ImGui::TreeNode("Camera")) {
		static Vector3 cameraPos = { 0.0f, 0.0f, -10.0f };
		static Vector3 cameraRot = { 0.0f, 0.0f, 0.0f };
		static Vector3 cameraScale = { 1.0f, 1.0f, 1.0f };

		if (ImGui::DragFloat3("Position", &cameraPos.x, 0.1f)) {
			camera_->SetPosition(cameraPos);
		}

		if (ImGui::DragFloat3("Rotation", &cameraRot.x, 0.01f)) {
			camera_->SetRotation(cameraRot);
		}

		if (ImGui::DragFloat3("Scale", &cameraScale.x, 0.01f, 0.01f, 10.0f)) {
			camera_->SetScale(cameraScale);
		}

		ImGui::TreePop();
	}

	// オプション設定の表示
	if (ImGui::TreeNode("Option Settings")) {
		ImGui::Checkbox("Is Option Mode", &isOptionMode_);

		if (isOptionMode_) {
			const char* optionSelectNames[] = { "Master", "BGM", "SE", "Quit" };
			int currentIndex = static_cast<int>(currentOptionSelect_);
			ImGui::Combo("Current Option Select", &currentIndex, optionSelectNames, static_cast<int>(Option::Select::Count));
		}

		ImGui::Separator();
		ImGui::Text("Volume Settings");
		ImGui::SliderFloat("Master Volume", masterVolume_, 0.0f, 1.0f);
		ImGui::SliderFloat("BGM Volume", bgmVolume_, 0.0f, 1.0f);
		ImGui::SliderFloat("SE Volume", seVolume_, 0.0f, 1.0f);

		ImGui::TreePop();
	}

	ImGui::End();

	// TitleUIの設定
	titleUI_->DrawImGui();

	display->DrawImGui();

	ImGui::Begin("FPS");
	float deltaTime = engine_->GetFPSObserver()->GetDeltatime();
	ImGui::Text("DeltaTime: %.3f ms", deltaTime * 1000.0f);
	ImGui::Text("FPS: %.1f", 1.0f / deltaTime);
	ImGui::End();
#endif

	engine_->DrawImGui();

	// ウィンドウへの描画終了
	window->PostDraw(cmdObj);
}

void TitleScene::UpdateCalculatedVolumes() {
	calculatedBgmVolume_ = (*bgmVolume_) * (*masterVolume_);
	calculatedSeVolume_ = (*seVolume_) * (*masterVolume_);
}
