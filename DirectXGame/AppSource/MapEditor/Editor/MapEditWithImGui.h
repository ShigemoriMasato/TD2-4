#pragma once
#include <Game/MapData/Data/MapData.h>

namespace MapEditor {

	class EditConfig {
	public:

		void Initialize();

		/// <summary>
		/// MapChip以外の編集
		/// </summary>
		/// <param name="mapData"></param>
		void DrawImGui(MapDataForBin& mapData);

		/// <summary>
		/// 要求するMapID(存在しない場合は新しく作成して渡すこと)
		/// </summary>
		/// <returns>mapID</returns>
		int RequestMap();

	private:

		int requestMapID_ = 0;
		char modelFilePathBuffer_[256] = {};

	};

}
