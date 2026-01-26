#include "ModelEditScene.h"
#include <GameCamera/DebugMousePos.h>

namespace{
	Vector2 GetPositionWithGrid(const Vector3& worldPos, float gridSize, Vector2 offset) {

		Vector2 snappedPos = {
			static_cast<float>(static_cast<int>(worldPos.x) / gridSize) * gridSize,
			static_cast<float>(static_cast<int>(worldPos.z) / gridSize) * gridSize
		};

		return snappedPos + offset;
	}
}

ModelEditScene::~ModelEditScene() {
	auto& io = ImGui::GetIO();
	io.IniFilename = "Assets/.EngineResource/imgui.ini";
}

void ModelEditScene::Initialize() {
	cameraController_ = std::make_unique<CameraController>();
	cameraController_->Initialize(input_);

	mcRender_ = std::make_unique<DebugMCRender>();
	auto drawData = drawDataManager_->GetDrawData(modelManager_->GetNodeModelData(0).drawDataIndex);
	mcRender_->Initialize(&mcData_, drawData);

	mapRender_ = std::make_unique<MapRender>();
	mapRender_->Initialize(drawData);

	typeEditor_ = std::make_unique<MapTypeEditor>();
	typeEditor_->Initialize(&mcData_, input_);

	textureEditor_ = std::make_unique<MapTextureEditor>();
	textureEditor_->Initialize(textureManager_, input_);

	auto& io = ImGui::GetIO();
	io.IniFilename = "Assets/.EngineResource/modelEdit.ini";
}

std::unique_ptr<IScene> ModelEditScene::Update() {
	input_->Update();
	cameraController_->Update();

	//カーソル座標
	Vector3 freeCursor = cameraController_->GetWorldPos();
	Vector2 gridCursor = GetPositionWithGrid(freeCursor, 1.0f, { 0.5f,0.5f });
	Vector4(freeCursor, 1.0f);
	//カーソル座標の割り当て
	typeEditor_->SetCursorPos(gridCursor);
	textureEditor_->SetCursorPos(gridCursor);

	//Alphaの設定
	if (typeEditor_->IsAnySelected()) {
		mcRender_->SetAlpha(0.8f);
		mapRender_->SetAlpha(0.3f);
	} else {
		mcRender_->SetAlpha(0.1f);
		mapRender_->SetAlpha(0.8f);
	}

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
	mcRender_->Draw(cameraController_->GetVPMatrix(), window.GetWindow());
	mapRender_->Draw(cameraController_->GetVPMatrix(), window.GetWindow());
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

	typeEditor_->DrawImGui(mcRender_->GetColorMap());
	textureEditor_->DrawImGui();
	cameraController_->DebugDraw();
#endif

	engine_->ImGuiDraw();
}
