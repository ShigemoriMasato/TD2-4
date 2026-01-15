#include "MapData.h"

bool MapDataForBin::Verify() const {
	bool valid = true;
	if (width <= 0 || height <= 0) {
		valid = false;
	}
	if (modelFilePath.empty()) {
		valid = false;
	}
	if (tileData.size() != static_cast<size_t>(width * height)) {
		assert(false && "MapDataForBin::Verify() failed: mapID is negative");
		valid = false;
	}
	if (mapID < 0) {
		assert(false && "MapDataForBin::Verify() failed: mapID is negative");
		valid = false;
	}
	return valid;
}

bool MapData::Verify() const {
	bool valid = true;
	if (width <= 0 || height <= 0) {
		valid = false;
	}
	if (modelFilePath.empty()) {
		valid = false;
	}
	if (tileGrid.size() != static_cast<size_t>(height)) {
		assert(false && "MapData::Verify() failed: tileGrid height mismatch");
		valid = false;
	} else {
		for (const auto& row : tileGrid) {
			if (row.size() != static_cast<size_t>(width)) {
				assert(false && "MapData::Verify() failed: tileGrid width mismatch");
				valid = false;
				break;
			}
		}
	}
	if (mapID < 0) {
		assert(false && "MapData::Verify() failed: mapID is negative");
		valid = false;
	}
	return valid;
}
