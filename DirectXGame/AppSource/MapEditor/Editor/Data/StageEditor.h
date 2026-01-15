#pragma once
#include <Game/MapData/Data/StageData.h>
#include <Game/MapData/Data/MapData.h>
#include <Tool/Binary/BinaryManager.h>

namespace MapEditor {

	class StageEditor {
	public:

		~StageEditor() {
			Save();
		}

		void Initialize();
		void DrawImGui(const std::vector<MapDataForBin>& mapData, std::map<TileType, ImU32> tileColorMap);

	private:

		void Load();
		void Save();

		std::vector<StageData> stageData_{};

		BinaryManager binaryManager_;
		const std::string stageSaveFile_ = "StageData";

		int editStageID_ = 0;
		int numberOfStages_ = 1;

		std::vector<const char*> directionNames_{};
	};

}
