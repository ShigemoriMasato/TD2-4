#include "../SearchFile.h"

namespace fs = std::filesystem;

std::vector<std::string> SearchFiles(const fs::path& directory, const std::string& extension) {
    std::vector<std::string> contents;

    if (!fs::exists(directory) || !fs::is_directory(directory)) {
        return {};
    }

    for (const auto& entry : fs::directory_iterator(directory)) {
        if (entry.is_regular_file() && entry.path().extension() == extension) {
            fs::path relativePath = entry.path().lexically_relative(directory);
            contents.push_back(relativePath.generic_string());
        }
    }

    return contents;
}

std::vector<std::string> SearchFileNames(const fs::path& directory) {
    std::vector<std::string> contents;

    if (!fs::exists(directory) || !fs::is_directory(directory)) {
        return {};
    }

    for (const auto& entry : fs::directory_iterator(directory)) {
        if (entry.is_directory()) {
            contents.push_back(entry.path().filename().generic_string());
        }
    }

    return contents;

}

std::vector<std::string> SerchFilePathsAddChild(const fs::path& directory, const std::string& extension) {
    std::vector<std::string> result;

    if (!fs::exists(directory) || !fs::is_directory(directory)) {
        return {};
    }

    for (const auto& entry : fs::recursive_directory_iterator(directory)) {
        if (entry.is_regular_file() && (entry.path().extension() == extension || extension.empty())) {
            fs::path relativePath = entry.path().lexically_relative(directory);
            result.push_back(relativePath.generic_string());
        }
    }

    return result;
}

bool IsBeFile(const std::string& filePath) {
    return fs::exists(filePath);
}
