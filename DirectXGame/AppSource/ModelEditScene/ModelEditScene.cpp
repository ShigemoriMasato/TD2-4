#include "ModelEditScene.h"
#include <GameCamera/DebugMousePos.h>

namespace {
	Vector2 GetPositionWithGrid(const Vector3& worldPos, float gridSize, Vector2 offset) {

		Vector2 snappedPos = {
			static_cast<float>(static_cast<int>(worldPos.x) / gridSize) * gridSize,
			static_cast<float>(static_cast<int>(worldPos.z) / gridSize) * gridSize
		};

		return snappedPos + offset;
	}
}

ModelEditScene::~ModelEditScene() {
#ifdef USE_IMGUI
	auto& io = ImGui::GetIO();
	io.IniFilename = "Assets/.EngineResource/imgui.ini";
#endif
}

void ModelEditScene::Initialize() {
	cameraController_ = std::make_unique<CameraController>();
	cameraController_->Initialize(input_);

	mcRender_ = std::make_unique<DebugMCRender>();
	auto drawData = drawDataManager_->GetDrawData(modelManager_->GetNodeModelData(0).drawDataIndex);
	mcRender_->Initialize(drawData);

	mapRender_ = std::make_unique<MapRender>();
	mapRender_->Initialize(drawData);

	typeEditor_ = std::make_unique<MapTypeEditor>();
	typeEditor_->Initialize(input_);

	textureEditor_ = std::make_unique<MapTextureEditor>();
	textureEditor_->Initialize(textureManager_, input_);

#ifdef USE_IMGUI
	auto& io = ImGui::GetIO();
	io.IniFilename = "Assets/.EngineResource/modelEdit.ini";
#endif

	textureManager_->LoadAllTextures();
}

std::unique_ptr<IScene> ModelEditScene::Update() {
	input_->Update();
	cameraController_->Update();

	//カーソル座標
	Vector3 freeCursor = cameraController_->GetWorldPos();
	Vector2 gridCursor = GetPositionWithGrid(freeCursor + Vector3(0.5f, 0.0f, 0.5f), 1.0f, {0.5f,0.5f});
	Vector4(freeCursor, 1.0f);
	//カーソル座標の割り当て
	typeEditor_->SetCursorPos(gridCursor);
	textureEditor_->SetCursorPos(gridCursor);

	//何処を触っているかの設定
	if (typeEditor_->IsAnySelected()) {
		whichEditMode_ = true;

		//他のエディタで編集できなくする
		textureEditor_->NonEdit();
	} else if (textureEditor_->isSomeSelected()) {
		whichEditMode_ = false;

		//他のエディタで編集できなくする
		typeEditor_->NonEdit();
	} else if (false/*デコレーション予定*/) {

	}

	//ステージナンバーの設定
	if (stageChanged_) {
		stageChanged_ = false;
		typeEditor_->SetCurrentStage(currentStage_);
		textureEditor_->SetCurrentStage(currentStage_);
	}

	//Alphaの設定
	if (whichEditMode_) {
		mcRender_->SetAlpha(0.8f);
		mapRender_->SetAlpha(0.2f);
	} else {
		mcRender_->SetAlpha(0.4f);
		mapRender_->SetAlpha(0.8f);
	}

	//値のすり合わせ
	auto mapSize = typeEditor_->GetGridPos();
	textureEditor_->SetMapSize(mapSize.first, mapSize.second);
	mapRender_->SetConfig(textureEditor_->GetTextureIndices());

	//更新
	typeEditor_->Update();
	textureEditor_->Update();

	//更新最後組
	mcRender_->Update();
	mapRender_->Update();

	return nullptr;
}

void ModelEditScene::Draw() {
	auto& display = *commonData_->display.get();
	auto& window = *commonData_->mainWindow.get();

	display.PreDraw(window.GetCommandObject(), true);
	mapRender_->Draw(cameraController_->GetVPMatrix(), window.GetWindow());
	mcRender_->Draw(cameraController_->GetVPMatrix(), typeEditor_->GetColorMap(), typeEditor_->GetCurrentMapChipData(), window.GetWindow());
	display.PostDraw(window.GetCommandObject());

	window.PreDraw(true);
	window.DrawDisplayWithImGui();

#ifdef USE_IMGUI
	Vector2 freeCursor = cameraController_->GetWorldPos();
	Vector2 gridCursor = GetPositionWithGrid(freeCursor, 1.0f, { 0.5f,0.5f });

	ImGui::Begin("Debug");
	ImGui::Text("Free Cursor x : %.2f, y : %.2f", freeCursor.x, freeCursor.y);
	ImGui::Text("Grid Cursor x : %.2f, y : %.2f", gridCursor.x, gridCursor.y);
	if (ImGui::Button("AlphaToggle")) {
		if (typeEditor_->IsAnySelected()) {
			typeEditor_->OtherSelected();
		}
	}
	ImGui::End();

	ImGui::Begin("File");
	ImGui::Text("CurrentStage: %d", currentStage_);
	ImGui::SameLine();
	if (ImGui::Button("-")) {
		stageChanged_ = true;
		currentStage_ = std::max(0, currentStage_ - 1);
	}
	ImGui::SameLine();
	if (ImGui::Button("+")) {
		stageChanged_ = true;
		currentStage_++;
	}

	if (ImGui::Button("Save")) {
		typeEditor_->Save();
		textureEditor_->TextureSave();
		textureEditor_->MapSave();
	}
	ImGui::End();

	typeEditor_->DrawImGui();
	textureEditor_->DrawImGui();
	cameraController_->DebugDraw();
#endif

	engine_->ImGuiDraw();
}
