#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H
#include <unordered_map>
#include <string>

class FontLoader {
public:

	void Initialize();

	int Load(const std::string& filePath, int fontSize = 64);

private:

	std::string FilePathChecker(const std::string& filePath);

	std::vector<char> text_;

	FT_Library ft_ = nullptr;

	std::unordered_map<std::string, FT_Face> fonts_;

};
