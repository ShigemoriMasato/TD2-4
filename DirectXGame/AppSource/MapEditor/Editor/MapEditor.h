#pragma once
#include <Game/MapData/MapDataManager.h>
#include <MapEditor/Editor/MapEditWithImGui.h>
#include <MapEditor/Editor/Data/Campus.h>

namespace MapEditor {

	class Editor {
	public:

		void Initialize(MapDataManager* mapDataManager);
		void Update();
		void Draw();
		void DrawImGui();

	private:

		MapDataManager* mapDataManager_ = nullptr;

		std::unique_ptr<EditConfig> mapEdit_ = nullptr;
		std::unique_ptr<Campus> campus_ = nullptr;

	};

}
