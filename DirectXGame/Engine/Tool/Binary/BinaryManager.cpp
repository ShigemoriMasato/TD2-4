#include "BinaryManager.h"
#include <filesystem>
#include <fstream>
#include <ostream>

namespace fs = std::filesystem;

void BinaryManager::Write(const std::string& fileName) {
	std::ofstream file(basePath + fileName, std::ios::binary);

	if (!file) {
		throw std::runtime_error("Failed to open file for writing: " + fileName);
	}

	file.write(binaryBuffer_.data(), binaryBuffer_.size());

	file.close();
}

BinaryData BinaryManager::Read(const std::string& fileName) {
	const std::string path = basePath + fileName;
	std::ifstream file(path, std::ios::binary);
	if (!file) {
		return {};
	}

	const auto fileSize = fs::file_size(path);
	std::string buffer;
	buffer.resize(static_cast<size_t>(fileSize));

	file.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));

	return buffer;
}
