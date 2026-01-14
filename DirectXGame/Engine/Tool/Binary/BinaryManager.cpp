#include "BinaryManager.h"
#include <fstream>

std::unique_ptr<BinaryInput> BinaryManager::input_ = std::make_unique<BinaryInput>();
std::unique_ptr<BinaryOutput> BinaryManager::output_ = std::make_unique<BinaryOutput>();
std::string const BinaryManager::basePath_ = "Assets/Binary/";

namespace fs = std::filesystem;

BinaryManager::BinaryManager() {
	fs::create_directories(basePath_);
}

BinaryManager::~BinaryManager() {
}

void BinaryManager::Write(std::string fileName) {

	std::ofstream file(basePath_ + fileName + ".sg", std::ios::binary);

	if (!file.is_open()) {
		return;
	}

	for (auto v : values_) {
		output_->WriteBinary(file, v.get());
	}

	values_.clear();

	file.close();
}

std::vector<std::shared_ptr<ValueBase>> BinaryManager::Read(std::string fileName) {

	std::ifstream file(basePath_ + fileName, std::ios::binary);
	if (!file.is_open()) {
		return {};
	}

	std::vector<std::shared_ptr<ValueBase>> ans;

	while(file.peek() != EOF) {
		auto val = input_->ReadBinary(file);
		if (!val) {
			break;
		}
		ans.push_back(val);
	}

	file.close();

	return ans;
}
