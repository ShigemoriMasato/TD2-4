#include "TitleScene.h"
#include <imgui/imgui.h>
#include <Utility/Color.h>

#include <02_ShigeScene/ShigeScene.h>

#include <../Engine/Assets/Audio/AudioManager.h>

TitleScene::TitleScene() {
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

	uint32_t handle = AudioManager::GetInstance().GetHandleByName("TitleScene.mp3");
	if (handle != 0) {
		AudioManager::GetInstance().Play(handle, calculatedBgmVolume_, true);
	}

	postEffect_ = std::make_unique<PostEffect>();
	postEffect_->Initialize(textureManager_, drawDataManager_->GetDrawData(commonData_->postEffectDrawDataIndex));
	postEffectConfig_.cmdObj = commonData_->cmdObject.get();
	postEffectConfig_.origin = commonData_->display->GetDisplay();
}

std::unique_ptr<IScene> TitleScene::Update() {

	// 上下キーで選択を変更
	bool upPressed =   input_->GetKeyState(DIK_UPARROW) && !input_->GetPreKeyState(DIK_UPARROW) || input_->GetKeyState(DIK_W) && !input_->GetPreKeyState(DIK_W);
	bool downPressed = input_->GetKeyState(DIK_DOWNARROW) && !input_->GetPreKeyState(DIK_DOWNARROW) || input_->GetKeyState(DIK_S) && !input_->GetPreKeyState(DIK_S);
	bool leftPressed = input_->GetKeyState(DIK_LEFTARROW) && !input_->GetPreKeyState(DIK_LEFTARROW) || input_->GetKeyState(DIK_A) && !input_->GetPreKeyState(DIK_A);
	bool rightPressed = input_->GetKeyState(DIK_RIGHTARROW) && !input_->GetPreKeyState(DIK_RIGHTARROW) || input_->GetKeyState(DIK_D) && !input_->GetPreKeyState(DIK_D);

	// オプションモード中の処理
	if (isOptionMode_) {
		// 上下キーでオプション内の選択を変更
		if (upPressed) {
			int currentIndex = static_cast<int>(currentOptionSelect_);
			currentIndex--;
			if (currentIndex < 0) {
				currentIndex = static_cast<int>(Option::Select::Count) - 1;
			}
			currentOptionSelect_ = static_cast<Option::Select>(currentIndex);

			uint32_t handle = AudioManager::GetInstance().GetHandleByName("CursorMove.mp3");
			if (handle != 0) {
				AudioManager::GetInstance().Play(handle, calculatedSeVolume_, false);
			}
		}

		if (downPressed) {
			int currentIndex = static_cast<int>(currentOptionSelect_);
			currentIndex++;
			if (currentIndex >= static_cast<int>(Option::Select::Count)) {
				currentIndex = 0;
			}
			currentOptionSelect_ = static_cast<Option::Select>(currentIndex);

			uint32_t handle = AudioManager::GetInstance().GetHandleByName("CursorMove.mp3");
			if (handle != 0) {
				AudioManager::GetInstance().Play(handle, calculatedSeVolume_, false);
			}
		}

		// 左右キーで音量を調整
		if (leftPressed) {
			switch (currentOptionSelect_) {
			case Option::Select::Master:
				*masterVolume_ -= 0.1f;
				break;
			case Option::Select::BGM:
				*bgmVolume_ -= 0.1f;
				break;
			case Option::Select::SE:
				*seVolume_ -= 0.1f;
				break;
			default:
				break;
			}

			// 音量を再計算
			UpdateCalculatedVolumes();

			// BGMの音量を即座に変更
			uint32_t bgmHandle = AudioManager::GetInstance().GetHandleByName("TitleScene.mp3");
			if (bgmHandle != 0) {
				AudioManager::GetInstance().SetVolume(bgmHandle, calculatedBgmVolume_);
			}

		}

		if (rightPressed) {
			switch (currentOptionSelect_) {
			case Option::Select::Master:
				*masterVolume_ += 0.1f;
				break;
			case Option::Select::BGM:
				*bgmVolume_ += 0.1f;
				break;
			case Option::Select::SE:
				*seVolume_ += 0.1f;
				break;
			default:
				break;
			}

			// 音量を再計算
			UpdateCalculatedVolumes();

			// BGMの音量を即座に変更
			uint32_t bgmHandle = AudioManager::GetInstance().GetHandleByName("TitleScene.mp3");
			if (bgmHandle != 0) {
				AudioManager::GetInstance().SetVolume(bgmHandle, calculatedBgmVolume_);
			}

		}

		*masterVolume_ = std::clamp(*masterVolume_, 0.0f, 1.0f);
		*bgmVolume_ = std::clamp(*bgmVolume_, 0.0f, 1.0f);
		*seVolume_ = std::clamp(*seVolume_, 0.0f, 1.0f);

		// Zキーで決定（Quitの場合はオプションモードを解除）
		if (input_->GetKeyState(DIK_Z) && !input_->GetPreKeyState(DIK_Z) || 
			input_->GetKeyState(DIK_SPACE) && !input_->GetPreKeyState(DIK_SPACE)) {
			if (currentOptionSelect_ == Option::Select::Quit) {
				isOptionMode_ = false;
				currentOptionSelect_ = Option::Select::Master;

				uint32_t handle = AudioManager::GetInstance().GetHandleByName("Decide.mp3");
				if (handle != 0) {
					AudioManager::GetInstance().Play(handle, calculatedSeVolume_, false);
				}
			}
		}
	} else {
		// 通常モード：メインメニューの選択
		titleUI_->UpdateSelection(upPressed, downPressed);

		// Zキーで決定
		if (input_->GetKeyState(DIK_Z) && !input_->GetPreKeyState(DIK_Z) || 
			input_->GetKeyState(DIK_SPACE) && !input_->GetPreKeyState(DIK_SPACE)) {
			Title::Select currentSelect = titleUI_->GetCurrentSelect();

			uint32_t handle = AudioManager::GetInstance().GetHandleByName("Decide.mp3");
			if (handle != 0) {
				AudioManager::GetInstance().Play(handle, calculatedSeVolume_, false);
			}

			// Startが選択されている場合はシーン遷移
			if (currentSelect == Title::Select::Start) {
				AudioManager::GetInstance().StopAll();
				return std::make_unique<ShigeScene>();
			}

			else if (currentSelect == Title::Select::Option) {
				isOptionMode_ = true;
				currentOptionSelect_ = Option::Select::Master;
			}

			// Quitが選択されている場合はアプリケーションを終了
			else if (currentSelect == Title::Select::Quit) {
				AudioManager::GetInstance().StopAll();
				commonData_->shouldQuit = true;
			}
		}
	}

	// カメラの行列更新
	camera_->MakeMatrix();

	// VP行列を取得してUIを更新
	Matrix4x4 vpMatrix = camera_->GetVPMatrix();
	titleUI_->Update(vpMatrix);

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
#endif

	// ウィンドウへの描画（displayの内容を転送）
	window->PreDraw(cmdObj, false);

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
