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

		bool preLeftMouse_ = false;
		bool preRightMouse_ = false;
		bool preUndo_ = false;
		int preMapID_ = 0;

		BinaryManager binaryManager_;

		std::vector<std::vector<TileType>> undoStack_;

	};

}
