#include "TitleUI.h"
#include <../Engine/Assets/Audio/AudioManager.h>
#include <Utility/Color.h>
#include <Utility/MatrixFactory.h>
#include <Utility/Easing.h>
#include <Scene/CommonData.h>
#include <GameObject/Player/Player.h>

#ifdef USE_IMGUI
#include <imgui/imgui.h>
#endif

void TitleUI::Initialize(SHEngine::DrawDataManager* drawDataManager, SHEngine::ModelManager* modelManager, CommonData* commonData) {
	drawDataManager_ = drawDataManager;
	modelManager_ = modelManager;
	commonData_ = commonData;

	// モデルパスの配列
	const std::array<const char*, kUICount> modelPaths = {"Assets/Model/Frame", "Assets/Model/Frame", "Assets/Model/Frame", "Assets/Model/UI/Title/Logo", "Assets/Model/UI/Title/Start", "Assets/Model/UI/Title/Option", "Assets/Model/UI/Title/Quit"};

	// デバッグ名の配列
	const std::array<const char*, kUICount> debugNames = {"TitleUI_Frame", "TitleUI_Frame2", "TitleUI_Frame3", "TitleUI_Logo", "TitleUI_Start", "TitleUI_Option", "TitleUI_Quit"};

	// 各UIの初期化
	for (size_t i = 0; i < kUICount; ++i) {
		// モデルの読み込み
		modelIDs_[i] = modelManager_->LoadModel(modelPaths[i]);

		// 描画設定
		renders_[i] = std::make_unique<SHEngine::RenderObject>(debugNames[i]);
		renders_[i]->Initialize();
		auto model = modelManager_->GetNodeModelData(modelIDs_[i]);
		auto drawData = drawDataManager_->GetDrawData(model.drawDataIndex);
		renders_[i]->SetDrawData(drawData);
		renders_[i]->psoConfig_.vs = "Simple.VS.hlsl";
		renders_[i]->psoConfig_.ps = "TexColor.PS.hlsl";
		renders_[i]->psoConfig_.isSwapChain = false; // displayに描画するのでfalseに変更
		renders_[i]->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER, "WVP");
		renders_[i]->CreateCBV(sizeof(Vector4), ShaderType::PIXEL_SHADER, "Color");
		renders_[i]->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "TextureIndex");
		renders_[i]->SetUseTexture(true);
		renders_[i]->instanceNum_ = 1;
	}

	currentSelect_ = Title::Select::Start;

	// Compassの初期化
	compassModelID_ = modelManager_->LoadModel("Assets/Model/UI/Title/Compass");
	compassRender_ = std::make_unique<SHEngine::RenderObject>("TitleUI_Compass");
	compassRender_->Initialize();
	{
		auto model = modelManager_->GetNodeModelData(compassModelID_);
		auto drawData = drawDataManager_->GetDrawData(model.drawDataIndex);
		compassRender_->SetDrawData(drawData);
		compassRender_->psoConfig_.vs = "Simple.VS.hlsl";
		compassRender_->psoConfig_.ps = "TexColor.PS.hlsl";
		compassRender_->psoConfig_.isSwapChain = false;
		compassRender_->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER, "WVP");
		compassRender_->CreateCBV(sizeof(Vector4), ShaderType::PIXEL_SHADER, "Color");
		compassRender_->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "TextureIndex");
		compassRender_->SetUseTexture(true);
		compassRender_->instanceNum_ = 1;
	}

	// ThumbsUpの初期化
	thumbsUpModelID_ = modelManager_->LoadModel("Assets/Model/UI/Title/thumbsUp");
	thumbsUpRender_ = std::make_unique<SHEngine::RenderObject>("TitleUI_ThumbsUp");
	thumbsUpRender_->Initialize();
	{
		auto model = modelManager_->GetNodeModelData(thumbsUpModelID_);
		auto drawData = drawDataManager_->GetDrawData(model.drawDataIndex);
		thumbsUpRender_->SetDrawData(drawData);
		thumbsUpRender_->psoConfig_.vs = "Simple.VS.hlsl";
		thumbsUpRender_->psoConfig_.ps = "TexColor.PS.hlsl";
		thumbsUpRender_->psoConfig_.isSwapChain = false;
		thumbsUpRender_->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER, "WVP");
		thumbsUpRender_->CreateCBV(sizeof(Vector4), ShaderType::PIXEL_SHADER, "Color");
		thumbsUpRender_->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "TextureIndex");
		thumbsUpRender_->SetUseTexture(true);
		thumbsUpRender_->instanceNum_ = 1;
	}
}

void TitleUI::StartCompassExitAnimation() {
	if (!compassExiting_) {
		compassExiting_ = true;
		compassExitTimer_ = 0.0f;
		compassExitStartScale_ = compassScale_;
	}
}

void TitleUI::UpdateSelection(bool upPressed, bool downPressed) {
	if (upPressed) {
		int currentIndex = static_cast<int>(currentSelect_);
		currentIndex--;

		if (currentIndex < 0) {
			currentIndex = static_cast<int>(Title::Select::Count) - 1;
		}
		currentSelect_ = static_cast<Title::Select>(currentIndex);

		AudioManager::GetInstance()->GetData("CursorMove.mp3")->Play();
	}

	if (downPressed) {
		int currentIndex = static_cast<int>(currentSelect_);
		currentIndex++;

		if (currentIndex >= static_cast<int>(Title::Select::Count)) {
			currentIndex = 0;
		}
		currentSelect_ = static_cast<Title::Select>(currentIndex);

		AudioManager::GetInstance()->GetData("CursorMove.mp3")->Play();
	}
}

void TitleUI::Update(const Matrix4x4& vpMatrix, float deltaTime) {
	int textureIndex = 0;

	for (size_t i = 0; i < kUICount; ++i) {
		Vector4 color = {1.0f, 1.0f, 1.0f, 1.0f};

		// 選択中の項目を赤色にする（Frame、Frame2、Frame3、Logoは除外、範囲内のときだけ）
		if (playerInRange_ && i > 3) {
			Title::Kinds kind = static_cast<Title::Kinds>(i);
			Title::Select selectFromKind = static_cast<Title::Select>(static_cast<int>(kind) - 4);
			if (selectFromKind == currentSelect_) {
				color = {1.0f, 0.0f, 0.0f, 1.0f};
			}
		}

		// WVP行列を作成
		Matrix4x4 world = Matrix::MakeAffineMatrix(scales_[i], rotations_[i], positions_[i]);
		Matrix4x4 wvp = world * vpMatrix;

		renders_[i]->CopyBufferData(0, &wvp, sizeof(Matrix4x4));
		renders_[i]->CopyBufferData(1, &color, sizeof(Vector4));
		renders_[i]->CopyBufferData(2, &textureIndex, sizeof(int));
	}

	if (player_) {
		Vector4 compassColor = { 1.0f, 1.0f, 1.0f, 1.0f };

		if (compassExiting_) {
			// 退場アニメーション中：色を緑にしてScaleをイージングで0に
			compassColor = { 0.0f, 1.0f, 0.0f, 1.0f };
			compassExitTimer_ += deltaTime;
			float t = std::clamp(compassExitTimer_ / compassExitDuration_, 0.0f, 1.0f);
			compassScale_ = lerp<Vector3>(compassExitStartScale_, { 0.0f, 0.0f, 0.0f }, t, EaseType::EaseInBack);

			// CompassのScaleが0になったらThumbsUpの登場アニメーションを開始
			if (t >= 1.0f && !thumbsUpEntering_ && !thumbsUpEnd_) {
				thumbsUpEntering_ = true;
				thumbsUpEnterTimer_ = 0.0f;
			}
		} else {
			// 通常のアニメーション（往復イージング）
			compassAnimTimer_ += deltaTime * compassAnimSpeed_;
			if (compassAnimTimer_ > 1.0f) { compassAnimTimer_ -= 1.0f; }
			const Vector3 scaleA{ 0.75f, 1.0f, 0.75f };
			const Vector3 scaleB{ 0.65f, 1.0f, 0.65f };
			compassScale_ = lerp_RoundTrip<Vector3>(scaleA, scaleB, compassAnimTimer_, EaseType::EaseInOutSine, EaseType::EaseInOutSine);
		}

		Vector3 playerPos = player_->GetTransform().position;
		Vector3 compassPos = playerPos + compassOffset_;
		Matrix4x4 world = Matrix::MakeAffineMatrix(compassScale_, compassRotation_, compassPos);
		Matrix4x4 wvp = world * vpMatrix;

		compassRender_->CopyBufferData(0, &wvp, sizeof(Matrix4x4));
		compassRender_->CopyBufferData(1, &compassColor, sizeof(Vector4));
		compassRender_->CopyBufferData(2, &textureIndex, sizeof(int));

		// ThumbsUp: 登場アニメーション
		if (thumbsUpEntering_ && !thumbsUpEnd_) {
			thumbsUpEnterTimer_ += deltaTime;
			float t = std::clamp(thumbsUpEnterTimer_ / thumbsUpEnterDuration_, 0.0f, 1.0f);
			thumbsUpScale_ = lerp<Vector3>({ 0.0f, 0.0f, 0.0f }, { 1.5f, 1.5f, 1.5f }, t, EaseType::EaseOutBack);
			thumbsUpRotation_.z = lerp<float>(0.9f, -0.2f, t, EaseType::EaseOutBack);

			// 登場完了→待機タイマー開始
			if (t >= 1.0f) {
				//thumbsUpEntering_ = false;
				thumbsUpWaitTimer_ = 0.0f;
				thumbsUpExiting_ = true;
			}
		}
		// ThumbsUp: 待機→退場へ移行
		//else if (!thumbsUpExiting_ && thumbsUpWaitTimer_ >= 0.0f) {
		//	thumbsUpWaitTimer_ += deltaTime;
		//	if (thumbsUpWaitTimer_ >= thumbsUpWaitDuration_) {
		//		thumbsUpExiting_ = true;
		//		thumbsUpExitTimer_ = 0.0f;
		//	}
		//}

		// ThumbsUp: 退場アニメーション
		if (thumbsUpExiting_ && !thumbsUpEnd_) {
			thumbsUpExitTimer_ += deltaTime;
			float t = std::clamp(thumbsUpExitTimer_ / thumbsUpExitDuration_, 0.0f, 1.0f);
			thumbsUpScale_ = lerp<Vector3>({ 1.5f, 1.5f, 1.5f }, { 0.0f, 0.0f, 0.0f }, t, EaseType::EaseInBack);
			thumbsUpRotation_.z = lerp<float>(-0.2f, 0.9f, t, EaseType::EaseInBack);
			if (t >= 0.9f) {
				thumbsUpExiting_ = false;
				thumbsUpEnd_ = true;
			}
		}

		// ThumbsUp: Playerの頭上に表示
		Vector3 thumbsUpPos = playerPos + thumbsUpOffset_;
		Matrix4x4 thumbsUpWorld = Matrix::MakeAffineMatrix(thumbsUpScale_, thumbsUpRotation_, thumbsUpPos);
		Matrix4x4 thumbsUpWvp = thumbsUpWorld * vpMatrix;
		Vector4 thumbsUpColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		thumbsUpRender_->CopyBufferData(0, &thumbsUpWvp, sizeof(Matrix4x4));
		thumbsUpRender_->CopyBufferData(1, &thumbsUpColor, sizeof(Vector4));
		thumbsUpRender_->CopyBufferData(2, &textureIndex, sizeof(int));
	}
}

void TitleUI::Draw(CmdObj* cmdObj) {
	for (size_t i = 0; i < kUICount; ++i) {
		renders_[i]->Draw(cmdObj);
	}
	if (player_) {
		compassRender_->Draw(cmdObj);
		
		if (!thumbsUpEnd_) {
			thumbsUpRender_->Draw(cmdObj);
		}
	}
}

#ifdef USE_IMGUI
void TitleUI::DrawImGui() {
	ImGui::Begin("Title UI Settings");

	static const char* uiNames[] = {"Frame", "Frame2", "Frame3", "Logo", "Start", "Option", "Quit"};
	static const char* selectNames[] = {"Start", "Option", "Quit"};

	for (size_t i = 0; i < kUICount; ++i) {
		if (ImGui::TreeNode(uiNames[i])) {
			ImGui::DragFloat3("Position", &positions_[i].x, 0.01f);
			ImGui::DragFloat3("Rotation", &rotations_[i].x, 0.01f);
			ImGui::DragFloat3("Scale", &scales_[i].x, 0.01f, 0.01f, 10.0f);
			ImGui::TreePop();
		}
	}

	ImGui::Separator();
	if (ImGui::TreeNode("Compass")) {
		ImGui::DragFloat3("Offset", &compassOffset_.x, 0.01f);
		ImGui::DragFloat3("Rotation", &compassRotation_.x, 0.01f);
		ImGui::DragFloat3("Scale", &compassScale_.x, 0.01f, 0.01f, 10.0f);
		ImGui::TreePop();
	}

	ImGui::Separator();
	if (ImGui::TreeNode("ThumbsUp")) {
		ImGui::DragFloat3("Offset", &thumbsUpOffset_.x, 0.01f);
		ImGui::DragFloat3("Rotation", &thumbsUpRotation_.x, 0.01f);
		ImGui::DragFloat3("Scale", &thumbsUpScale_.x, 0.01f, 0.01f, 10.0f);
		ImGui::TreePop();
	}

	ImGui::Separator();
	int currentSelectIndex = static_cast<int>(currentSelect_);
	if (ImGui::Combo("Current Selection", &currentSelectIndex, selectNames, static_cast<int>(Title::Select::Count))) {
		currentSelect_ = static_cast<Title::Select>(currentSelectIndex);
	}

	ImGui::End();
}
#endif