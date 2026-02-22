#include "ShigeScene.h"
#include <imgui/imgui.h>
#include <Utility/Color.h>

void ShigeScene::Initialize() {
	pieceRender_ = std::make_unique<PieceRender>();
	pieceRender_->Initialize(modelManager_, drawDataManager_);

	//とりあえず適当に配置
	Piece piece;
	Item item;
	item.modelID = 0;
	piece.SetItem(item);
	pieces_.push_back(piece);

	camera_ = std::make_unique<DebugCamera>();
	camera_->Initialize(input_);

	grid_ = std::make_unique<Grid>();
	grid_->Initialize(drawDataManager_);
}

std::unique_ptr<IScene> ShigeScene::Update() {
	camera_->Update();
	Vector3 cameraPos = camera_->GetCenter();
	grid_->Update(cameraPos, camera_->GetVPMatrix());

	pieceRender_->SetPiece(pieces_);
	pieceRender_->Update(camera_->GetVPMatrix());

	return nullptr;
}

void ShigeScene::Draw() {
	auto window = commonData_->mainWindow.second.get();
	auto display = commonData_->display.get();
	auto cmdObj = commonData_->cmdObject.get();

	display->PreDraw(cmdObj, true);

	grid_->Draw(cmdObj);
	pieceRender_->Draw(cmdObj);

	display->PostDraw(cmdObj);

	window->PreDraw(cmdObj);

	//ここ以外で記述する場合、ifdefを忘れないようにすること
#ifdef USE_IMGUI


	display->DrawImGui();

	ImGui::Begin("FPS");
	float deltaTime = engine_->GetFPSObserver()->GetDeltatime();
	ImGui::Text("DeltaTime: %.3f ms", deltaTime * 1000.0f);
	ImGui::Text("FPS: %.1f", 1.0f / deltaTime);
	ImGui::End();
#endif


	engine_->DrawImGui();
	window->PostDraw(cmdObj);
}
