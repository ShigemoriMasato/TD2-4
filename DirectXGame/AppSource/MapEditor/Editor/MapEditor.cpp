#include "MapEditor.h"

using namespace MapEditor;

void Editor::Initialize(MapDataManager* mapDataManager) {
	mapDataManager_ = mapDataManager;

	mapEdit_ = std::make_unique<EditConfig>();
	mapEdit_->Initialize();

	campus_ = std::make_unique<Campus>();
	campus_->Initialize();
}

void Editor::Update() {

}

void Editor::Draw() {
	//todo 現在編集中のマップのスクリーンショットを描画する
}

void Editor::DrawImGui() {
#ifdef USE_IMGUI
	int require = mapEdit_->RequestMap();

	MapDataForBin* currentMapData = mapDataManager_->GetMapData(require);
	
	mapEdit_->DrawImGui(*currentMapData);

	campus_->DrawImGui(*currentMapData);

#endif
}
