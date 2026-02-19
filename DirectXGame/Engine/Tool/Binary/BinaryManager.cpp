#include "BinaryManager.h"
#include <filesystem>
#include <fstream>
#include <ostream>

namespace fs = std::filesystem;

template<>
void BinaryManager::RegisterOutput<std::string>(std::string* data) {
	constexpr TypeID id = TypeIDResolver<std::string>::id;
	size_t size = data->size();

	binaryBuffer_.append(reinterpret_cast<const char*>(&id), sizeof(id));
	binaryBuffer_.append(reinterpret_cast<const char*>(&size), sizeof(size));
	binaryBuffer_.append(data->data(), size);
}

template<>
std::string BinaryManager::Reverse<std::string>(BinaryData& buffer) {
	std::string value;

	if (buffer.size() < headerSize) {
		return value;
	}

	TypeID id;
	size_t size;
	std::memcpy(&id, buffer.data(), idSize);
	std::memcpy(&size, buffer.data() + idSize, sizeSize);

	if(id != TypeIDResolver<std::string>::id) {
		return value;
	}

	if(buffer.size() < headerSize + size) {
		return value;
	}

	value.resize(size);
	std::memcpy(value.data(), buffer.data() + headerSize, size);
	buffer.erase(0, headerSize + size);

	return value;
}

void BinaryManager::Write(const std::string& fileName) {
	std::ofstream file(fileName, std::ios::binary);

	if (!file) {
		throw std::runtime_error("Failed to open file for writing: " + fileName);
	}

	file.write(binaryBuffer_.data(), binaryBuffer_.size());

	file.close();
}

BinaryData BinaryManager::Read(const std::string& fileName) {
	std::ifstream file(fileName, std::ios::binary);
	if (!file) {
		return {};
	}

	const auto fileSize = fs::file_size(fileName);
	std::string buffer;
	buffer.resize(fileSize);

	file.read((char*)buffer.data(), fs::file_size(fileName));

	return buffer;
}
