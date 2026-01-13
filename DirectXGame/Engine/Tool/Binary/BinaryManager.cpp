#include "BinaryManager.h"
#include <fstream>

std::unique_ptr<BinaryInput> BinaryManager::input = std::make_unique<BinaryInput>();
std::unique_ptr<BinaryOutput> BinaryManager::output = std::make_unique<BinaryOutput>();

namespace fs = std::filesystem;

BinaryManager::BinaryManager() {
	fs::create_directories(basePath);
}

BinaryManager::~BinaryManager() {
}

void BinaryManager::Write(std::string fileName) {

	std::ofstream file(basePath + fileName + ".sg", std::ios::binary);

	if (!file.is_open()) {
		return;
	}

	for (auto v : values) {
		output->WriteBinary(file, v.get());
	}

	values.clear();

	file.close();
}

std::vector<std::shared_ptr<ValueBase>> BinaryManager::Read(std::string fileName) {

	std::ifstream file(basePath + fileName, std::ios::binary);
	if (!file.is_open()) {
		return {};
	}

	std::vector<std::shared_ptr<ValueBase>> ans;

	while(file.peek() != EOF) {
		auto val = input->ReadBinary(file);
		if (!val) {
			break;
		}
		ans.push_back(val);
	}

	file.close();

	return ans;
}
