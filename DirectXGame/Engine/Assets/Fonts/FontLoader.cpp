#include "FontLoader.h"

#include <algorithm>
#include <cassert>
#include <filesystem>
#include <Utility/Vector.h>

void FontLoader::Initialize() {
	Logger logger = getLogger("Engine");
	logger->info("FontLoader Initialize");

	FT_Error err = FT_Init_FreeType(&ft_);
	if (err) {
		logger->error("Failed to initialize FreeType Library");
		assert(false);
		return;
	}
}

int FontLoader::Load(const std::string& filePath, int fontSize) {
	//stringの整合性チェック
	std::string factPath = FilePathChecker(filePath);
	std::string cacheFileName = filePath + "_" + std::to_string(fontSize);

	if (fontIndex_.find(cacheFileName) != fontIndex_.end()) {
		return fontIndex_[cacheFileName];
	}

	logger_->info("FontLoader Load: {}", factPath);

	auto values = binaryManager_.Read(cachePath_ + cacheFileName);
	IntermediateFontData fontBuffer;

	if (values.empty()) {
		//フォントデータを新規作成
		fontBuffer = CreateFontBuffer(factPath, fontSize);

		//キャッシュを作成
	} else {
		int index = 0;

		//AtlasData
		int size = BinaryManager::Reverse<int>(values[index++].get());
		fontBuffer.atlasData.resize(atlas_width_ * atlas_height_, 0);
		for (int i = 0; i < size; ++i) {
			fontBuffer.atlasData[i] = BinaryManager::Reverse<uint8_t>(values[index++].get());
		}

		//CharPositions
		int charPosSize = BinaryManager::Reverse<int>(values[index++].get());
		for (int i = 0; i < charPosSize; ++i) {
			CharPosition charPos;
			wchar_t key = static_cast<wchar_t>(BinaryManager::Reverse<int>(values[index++].get()));
			charPos.uvStart = BinaryManager::Reverse<Vector2>(values[index++].get());
			charPos.uvEnd = BinaryManager::Reverse<Vector2>(values[index++].get());
			fontBuffer.charPositions[key] = charPos;
		}
	}



	return 0;
}

FontLoader::IntermediateFontData FontLoader::CreateFontBuffer(const std::string& filePath, int fontSize) {
	IntermediateFontData data{};
	data.atlasData.resize(atlas_width_ * atlas_height_, 0);

	FT_Face face = nullptr;

	FT_New_Face(ft_, filePath.c_str(), 0, &face);
	FT_Set_Pixel_Sizes(face, 0, fontSize);

	//文字の最大の高さを取得
	int maxTop = 0;
	int maxBottom = 0;

	for (wchar_t c : text_) {
		FT_Load_Char(face, c, FT_LOAD_RENDER);
		auto& g = face->glyph;

		maxTop = std::max(maxTop, g->bitmap_top);
		maxBottom = std::max(maxBottom, (int)g->bitmap.rows - g->bitmap_top);
	}

	int height = maxTop + maxBottom;

	//文字を配置する位置
	int dest_x = 0;
	int dest_y = 0;

	for (const wchar_t& c : text_) {
		//フォントの文字を持ってくる
		FT_Load_Char(face, c, FT_LOAD_RENDER);

		auto& bitmap = face->glyph->bitmap;

		//改行処理
		if (dest_x + (int)bitmap.width >= atlas_width_) {
			dest_x = 0;
			dest_y += height;

			//Atlasの高さを超えたら終了
			if (dest_y + height >= atlas_height_) {
				break;
			}
		}

		for (int y = 0; y < (int)bitmap.rows; ++y) {
			for (int x = 0; x < (int)bitmap.width; ++x) {
				uint8_t alpha = bitmap.buffer[y * bitmap.pitch + x];
				if (alpha == 0) continue;

				//下線を揃えるためにmaxTopを基準にしてyをオフセットさせる
				int offsetY = maxTop - face->glyph->bitmap_top;
				int targetIndex = (dest_y + offsetY + y) * atlas_width_ + (dest_x + x);

				data.atlasData[targetIndex] = alpha;
				data.lastUsedIndex = std::max(data.lastUsedIndex, targetIndex);

				//UV座標を登録
				CharPosition charPos;
				charPos.uvStart = Vector2(
					static_cast<float>(dest_x) / static_cast<float>(atlas_width_),
					static_cast<float>(dest_y + offsetY) / static_cast<float>(atlas_height_)
				);
				charPos.uvEnd = Vector2(
					static_cast<float>(dest_x + bitmap.width) / static_cast<float>(atlas_width_),
					static_cast<float>(dest_y + offsetY + bitmap.rows) / static_cast<float>(atlas_height_)
				);
				data.charPositions[c] = charPos;
			}
		}

		//次の文字の位置を更新
		dest_x += bitmap.width + 2;
	}

	return data;
}

void FontLoader::CreateCache(const IntermediateFontData& data, const std::string& cacheFileName) {
	if (!std::filesystem::exists(cachePath_)) {
		std::filesystem::create_directories(cachePath_);
	}

	int size = static_cast<int>(data.atlasData.size());
	binaryManager_.RegisterOutput(size);
	for (int i = 0; i < size; ++i) {
		binaryManager_.RegisterOutput(data.atlasData[i]);
	}
	binaryManager_.RegisterOutput(static_cast<int>(data.charPositions.size()));
	for (const auto& [key, value] : data.charPositions) {
		binaryManager_.RegisterOutput(static_cast<int>(key));
		binaryManager_.RegisterOutput(value.uvStart);
		binaryManager_.RegisterOutput(value.uvEnd);
	}
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

void FontLoader::CreateCache(const std::string& cacheFileName, const IntermediateFontData& data) {

	//AtlasData
	binaryManager_.RegisterOutput(static_cast<int>(data.lastUsedIndex));
	for (int i = 0; i < data.lastUsedIndex; ++i) {
		binaryManager_.RegisterOutput(data.atlasData[i]);
	}

	//CharPositions
	binaryManager_.RegisterOutput(static_cast<int>(data.charPositions.size()));
	for (const auto& [key, value] : data.charPositions) {
		binaryManager_.RegisterOutput(static_cast<int>(key));
		binaryManager_.RegisterOutput(value.uvStart);
		binaryManager_.RegisterOutput(value.uvEnd);
	}

	binaryManager_.Write(cachePath_ + cacheFileName);
}

void FontLoader::LoadCache(const std::string& cacheFileName, IntermediateFontData& data) {
	auto values = binaryManager_.Read(cachePath_ + cacheFileName);

	if (values.empty()) {
		return;
	}

	int index = 0;
	//AtlasData
	data.lastUsedIndex = BinaryManager::Reverse<int>(values[index++].get());
	data.atlasData.resize(atlas_width_ * atlas_height_, 0);
	for (int i = 0; i < data.lastUsedIndex; ++i) {
		data.atlasData[i] = BinaryManager::Reverse<uint8_t>(values[index++].get());
	}
	//CharPositions
	int charPosSize = BinaryManager::Reverse<int>(values[index++].get());
	for (int i = 0; i < charPosSize; ++i) {
		wchar_t key = static_cast<wchar_t>(BinaryManager::Reverse<int>(values[index++].get()));
		Vector2 uvStart = BinaryManager::Reverse<Vector2>(values[index++].get());
		Vector2 uvEnd = BinaryManager::Reverse<Vector2>(values[index++].get());
		CharPosition charPos;
		charPos.uvStart = uvStart;
		charPos.uvEnd = uvEnd;
		data.charPositions[key] = charPos;
	}
}
