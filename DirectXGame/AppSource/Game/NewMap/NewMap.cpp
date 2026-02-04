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

	if (dir == Direction::Back) {
		Vector2 middle = { float(mapChipData[0].size()) / 2.0f, float(mapChipData.size()) / 2.0f };
		for (auto& [modelIndex, transforms] : decorations) {
			for (auto& trans : transforms) {
				// 中心を原点に移動
				Vector3 pos = trans.position - Vector3(middle.x, 0.0f, middle.y);
				pos = Vector3(-pos.x, pos.y, -pos.z);
				// 再び元
				trans.position = pos + Vector3(middle.x, 0.0f, middle.y) - Vector3(1.0f, 0.0f, 1.0f);

				//回転
				trans.rotate.y += 3.14159f;
			}
		}
	}
}
