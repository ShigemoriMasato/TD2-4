#pragma once
#include <string>
#include <vector>
#include <filesystem>

std::vector<std::string> SearchFiles(const std::filesystem::path& directory, const std::string& extension);

std::vector<std::string> SearchFileNames(const std::filesystem::path& directory);

std::vector<std::string> SearchFilePathsAddChild(const std::filesystem::path& directory, const std::string& extension);

bool IsBeFile(const std::string& filePath);
