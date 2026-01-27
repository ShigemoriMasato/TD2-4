#pragma once
#include <Assets/Model/ModelManager.h>
#include <Camera/DebugCamera.h>
#include <Game/MapData/Data/MapData.h>
#include <Render/RenderObject.h>
#include <map>

class MapModelEditor {
public:

	void Initialize(ModelManager* modelManager, DrawDataManager* drawDataManager);
	void Update();
	void Draw(Window* window);
	void DrawImGui();

private:

	void Save();
	void Load();

	std::map<TileType, std::string> tileTypeStr_;

	ModelManager* modelManager_ = nullptr;
	DrawDataManager* drawDataManager_ = nullptr;
	std::unique_ptr<DebugCamera> camera_ = nullptr;

	std::map<TileType, std::vector<std::string>> modelPaths_;

	TileType selectedTileType_ = TileType::Air;
	int selectedModelIndex_ = 0;

	char willLoadPath_[256] = "";

	std::string errorMsg_ = "";


	NodeModelData currentModel_;

	std::unique_ptr<RenderObject> renderObject_ = nullptr;

	BinaryManager binManager_;
};
