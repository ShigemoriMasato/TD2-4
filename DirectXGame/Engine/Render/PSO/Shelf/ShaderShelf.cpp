#include "ShaderShelf.h"
#include <Utility/ConvertString.h>
#include <Utility/SearchFile.h>
#include <Core/DXDevice.h>
#include <cassert>

using namespace SHEngine::PSO;

namespace fs = std::filesystem;

ShaderShelf::ShaderShelf(DXDevice* device) {
	device_ = device;

	compileVersions_[0] = L"vs_6_0"; // Vertex Shader
	compileVersions_[1] = L"ps_6_0"; // Pixel Shader

	if (!binManager_.Boot(saveFile_)) {
		return;
	}

	for (int i = 0; i < static_cast<int>(ShaderType::Count); ++i) {
		int shaderCount = binManager_.Reverse<int>();
		for (int j = 0; j < shaderCount; ++j) {
			std::string name = binManager_.Reverse<std::string>();
			GetShaderBytecode(static_cast<ShaderType>(i), name);
		}
	}
}

ShaderShelf::~ShaderShelf() {
	binManager_.Boot(saveFile_);

	for (int i = 0; i < static_cast<int>(ShaderType::Count); ++i) {
		int shaderCount = int(shaderBytecodes_[i].size());
		binManager_.Register(&shaderCount);
		for (const auto& [name, bytecode] : shaderBytecodes_[i]) {
			binManager_.Register(&name);
		}
	}

	binManager_.Write(saveFile_);
}

std::list<D3D12_SHADER_BYTECODE> ShaderShelf::GetShaderBytecodes(ShaderType shaderType) {
	std::list<D3D12_SHADER_BYTECODE> ans;

	for (const auto& [name, bytecode] : shaderBytecodes_[static_cast<size_t>(shaderType)]) {
		ans.push_back(bytecode);
	}

	return ans;
}

std::list<std::string> ShaderShelf::GetShaderNames(ShaderType shaderType) {
	std::list<std::string> ans;

	for (const auto& [name, bytecode] : shaderBytecodes_[static_cast<size_t>(shaderType)]) {
		ans.push_back(name);
	}

	return ans;
}

D3D12_SHADER_BYTECODE ShaderShelf::GetShaderBytecode(ShaderType shaderType, std::string shaderName) {
	//登録されていなければCompileする
	if (shaderBytecodes_[static_cast<int>(shaderType)][shaderName].pShaderBytecode == D3D12_SHADER_BYTECODE().pShaderBytecode) {
		RegisterShaderByteCode(shaderName, shaderType);
	}
	return shaderBytecodes_[static_cast<int>(shaderType)][shaderName];
}

void ShaderShelf::RegisterShaderByteCode(std::string shaderName, ShaderType shaderType) {
	IDxcBlob* blobBuffer = device_->CompileShader(basePath_.string() + "/" + shaderName, shaderType);

	shaderBlobs_.push_back(blobBuffer);

	shaderBytecodes_[int(shaderType)][shaderName] = { blobBuffer->GetBufferPointer(), blobBuffer->GetBufferSize() };
}
