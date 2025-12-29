#include "PSOEditor.h"

PSOEditor::PSOEditor(ID3D12Device* device) {
	logger_ = getLogger("Engine");

	psoManager_ = std::make_unique<PSOManager>(device);
}

void PSOEditor::Initialize(ID3D12Device* device) {
	psoManager_->Initialize();
}

void PSOEditor::SetPSO(ID3D12GraphicsCommandList* commandList, const PSOConfig& config) {
	commandList->SetGraphicsRootSignature(psoManager_->GetRootSignature(config.rootConfig));
	commandList->IASetPrimitiveTopology(config.topology);
	commandList->SetPipelineState(psoManager_->GetPSO(config));

	nowConfig_ = config;
}

void PSOEditor::FrameInitialize(ID3D12GraphicsCommandList* commandList) {
	nowConfig_ = {};
	commandList->SetGraphicsRootSignature(psoManager_->GetRootSignature(nowConfig_.rootConfig));
	commandList->SetPipelineState(psoManager_->GetPSO(nowConfig_));
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
