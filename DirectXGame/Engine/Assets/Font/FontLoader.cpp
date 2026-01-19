#include "FontLoader.h"

#include <algorithm>

void FontLoader::Initialize() {
	FT_Init_FreeType(&ft_);
}

int FontLoader::Load(const std::string& filePath, int fontSize) {
	FT_Face face = nullptr;

	//stringの整合性チェック
	std::string factPath{};
	

	FT_New_Face(ft_, factPath.c_str(), 0, &face);
	FT_Set_Pixel_Sizes(face, 0, fontSize);

	int penX = 0;
	int maxTop = 0;
	uint32_t maxBottom = 0;

	for (char c : text_) {
		FT_Load_Char(face, c, FT_LOAD_RENDER);
		auto& g = face->glyph;

		penX += g->advance.x >> 6;

		maxTop = std::max(maxTop, g->bitmap_top);
		maxBottom = std::max(maxBottom, g->bitmap.rows - g->bitmap_top);
	}

	int width = penX;
	int height = maxTop + maxBottom;

	std::vector<uint8_t> image(width * height * 4, 0);

	penX = 0;
	int baseline = maxTop;

	for (char c : text_) {
		FT_Load_Char(face, c, FT_LOAD_RENDER);
		auto& g = face->glyph;
		auto& bmp = g->bitmap;

		int x0 = penX + g->bitmap_left;
		int y0 = baseline - g->bitmap_top;

		for (uint32_t y = 0; y < bmp.rows; y++) {
			for (uint32_t x = 0; x < bmp.width; x++) {
				uint8_t a = bmp.buffer[y * bmp.pitch + x];
				if (a == 0) continue;

				int dst = ((y0 + y) * width + (x0 + x)) * 4;
				image[dst + 0] = 255;
				image[dst + 1] = 255;
				image[dst + 2] = 255;
				image[dst + 3] = a;
			}
		}

		penX += g->advance.x >> 6;
	}

	return 0;
}

std::string FontLoader::FilePathChecker(const std::string& filePath) {
	bool notAssets = false;

	std::string basePath = "Assets/";

	if (filePath.length() < basePath.length()) {
		notAssets = true;
	} else {
		for (int i = 0; i < int(basePath.length()); ++i) {
			if (filePath[i] != basePath[i]) {
				notAssets = true;
			}
		}
	}

	if (notAssets) {
		return "Assets/Font/" + filePath;
	} else {
		return filePath;
	}

	return {};
}
