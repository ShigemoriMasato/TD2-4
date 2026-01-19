#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H
#include <unordered_map>
#include <string>

#include <Tool/Binary/BinaryManager.h>

class FontLoader {
public:

	void Initialize();

	int Load(const std::string& filePath, int fontSize = 64);

private:

	struct CharPosition {
		Vector2 uvStart;
		Vector2 uvEnd;
	};

	struct FontData {
		int fontSize;
		int textureIndex;
		std::unordered_map<wchar_t, CharPosition> charPositions;
	};

	struct IntermediateFontData {
		std::vector<uint8_t> atlasData;
		std::unordered_map<wchar_t, CharPosition> charPositions;
		int lastUsedIndex;
	};

private:

	IntermediateFontData CreateFontBuffer(const std::string& filePath, int fontSize);

	std::string FilePathChecker(const std::string& filePath);

	void CreateCache(const std::string& cacheFileName, const IntermediateFontData& data);
	void LoadCache(const std::string& cacheFileName, IntermediateFontData& data);

private://フォント関係
	std::vector<wchar_t> text_;
	FT_Library ft_ = nullptr;
	std::unordered_map<std::string, FT_Face> fonts_;

private://入出力関係
	BinaryManager binaryManager_;
	std::string basePath_ = "Assets/Fonts/";
	std::string cachePath_ = "Assets/Fonts/cache/";

private://アトラス関係
	const int atlas_width_ = 2048;
	const int atlas_height_ = 2048;

private://フォントデータ
	std::unordered_map<std::string, int> fontIndex_;
	std::vector<FontData> fontData_;

private://Debug
	Logger logger_;
};
