#include "NewMap.h"
#include <ModelEditScene/Editor/Texture/MapTextureEditor.h>

namespace {

	template<typename T>
	std::vector<std::vector<T>> rot(const std::vector<std::vector<T>>& vec, Direction dir) {
		if(vec.empty() || vec[0].empty()) {
			return {};
		}

		std::vector<std::vector<T>> rotated{};

		size_t oldHeight = vec.size();
		size_t oldWidth = vec[0].size();

		switch (dir) {
		case Direction::Front:
			return vec;
		case Direction::Right:

			rotated.resize(oldWidth, std::vector<T>(oldHeight));
			for (size_t y = 0; y < oldHeight; ++y) {
				for (size_t x = 0; x < oldWidth; ++x) {
					rotated[oldWidth - 1 - x][y] = vec[y][x];
				}
			}

			break;
		case Direction::Back:

			rotated.resize(oldHeight, std::vector<T>(oldWidth));
			for (size_t y = 0; y < oldHeight; ++y) {
				for (size_t x = 0; x < oldWidth; ++x) {
					rotated[oldHeight - 1 - y][oldWidth - 1 - x] = vec[y][x];
				}
			}

			break;
		case Direction::Left:

			rotated.resize(oldWidth, std::vector<T>(oldHeight));
			for (size_t y = 0; y < oldHeight; ++y) {
				for (size_t x = 0; x < oldWidth; ++x) {
					rotated[x][oldHeight - 1 - y] = vec[y][x];
				}
			}

			break;
		}

		return rotated;
	}

}

void NewMap::Rotate(Direction dir) {
	mapChipData = rot(mapChipData, dir);
	renderData = rot(renderData, dir);
}
