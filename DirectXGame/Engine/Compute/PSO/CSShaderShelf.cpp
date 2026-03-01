#include "CSShaderShelf.h"

using namespace SHEngine::PSO;

void CSShaderShelf::Initialize(DXDevice* device) {
    shaderBytecodeMap_.clear();
	device_ = device;
}

D3D12_SHADER_BYTECODE SHEngine::PSO::CSShaderShelf::GetShaderBytecode(const std::string& name) {
}
