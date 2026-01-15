#pragma once
#include "Pencil.h"

namespace MapEditor {

	class Campus {
	public:

		~Campus() {
			Save();
		}

		void Initialize();
		void DrawImGui(MapDataForBin& data);

	private:

		void Save();
		void Load();

		std::map<TileType, ImU32> tileColorMap_;

		Pencil pencil_;

		int tileSize_ = 32;

		BinaryManager binaryManager_;

	};

}
