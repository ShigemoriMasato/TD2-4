#include "MapData.h"

bool MapData::Verify() const {
	bool valid = true;
	if (width <= 0 || height <= 0) {
		valid = false;
	}
	if (modelFilePath.empty()) {
		valid = false;
	}
	if (tileData.size() != static_cast<size_t>(width * height)) {
		assert(false && "MapData::Verify() failed: mapID is negative");
		valid = false;
	}
	if (mapID < 0) {
		assert(false && "MapData::Verify() failed: mapID is negative");
		valid = false;
	}
	return valid;
}
