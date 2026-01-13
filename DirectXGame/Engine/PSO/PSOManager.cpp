#include "PSOManager.h"
#include <cassert>

std::unordered_map<D3D12_PRIMITIVE_TOPOLOGY, D3D12_PRIMITIVE_TOPOLOGY_TYPE> PSOManager::topologyMap_{};

PSOManager::PSOManager(ID3D12Device* device) {
	shaderShelf_ = std::make_unique<ShaderShelf>();
	depthStencilShelf_ = std::make_unique<DepthStencilShelf>();
	blendStateShelf_ = std::make_unique<BlendStateShelf>();
	rasterizerShelf_ = std::make_unique<RasterizerShelf>();
	rootSignatureShelf_ = std::make_unique<RootSignatureShelf>(device);
	inputLayoutShelf_ = std::make_unique<InputLayoutShelf>();
	binaryManager_ = std::make_unique<BinaryManager>();

	logger_ = getLogger("Engine");

	device_ = device;

	topologyMap_[D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST] = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	topologyMap_[D3D_PRIMITIVE_TOPOLOGY_LINELIST] = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
	topologyMap_[D3D_PRIMITIVE_TOPOLOGY_POINTLIST] = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
}

PSOManager::~PSOManager() {
	for(const auto& [config, pso] : psoMap_) {
		pso->Release();
	}
}

void PSOManager::Initialize() {
	//PSOをすべて廃棄
	for (auto& [config, pso] : psoMap_) {
		if (pso) {
			pso->Release();
		}
	}
	psoMap_.clear();
}

void PSOManager::CreatePSO(PSOConfig config) {
	//defaultとして設定したPSOを持ってくる
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	//あんまり変わらないやつ
	psoDesc.NumRenderTargets = 1;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	//Configにあるやつ
	psoDesc.pRootSignature = rootSignatureShelf_->GetRootSignature(config.rootConfig);
	psoDesc.VS = shaderShelf_->GetShaderBytecode(ShaderType::VERTEX_SHADER, config.vs);
	psoDesc.PS = shaderShelf_->GetShaderBytecode(ShaderType::PIXEL_SHADER, config.ps);
	psoDesc.DepthStencilState = depthStencilShelf_->GetDepthStencilDesc(config.depthStencilID);
	psoDesc.BlendState = blendStateShelf_->GetBlendState(config.blendID);
	psoDesc.RasterizerState = rasterizerShelf_->GetRasterizerDesc(config.rasterizerID);
	psoDesc.InputLayout = inputLayoutShelf_->GetInputLayoutDesc(config.inputLayoutID);

	psoDesc.RTVFormats[0] = config.isSwapChain ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.PrimitiveTopologyType = topologyMap_[config.topology];

	ID3D12PipelineState* pso = nullptr;

	HRESULT hr = device_->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pso));

	if (FAILED(hr)) {
		assert(false && "Failed to create PSO");
	}

	psoMap_[config] = pso;
}

ID3D12PipelineState* PSOManager::GetPSO(const PSOConfig& config) {
	auto it = psoMap_.find(config);
	if (it != psoMap_.end()) {
		return it->second;
	} else {
		CreatePSO(config);
		return GetPSO(config);
	}
}

ID3D12RootSignature* PSOManager::GetRootSignature(const RootSignatureConfig& config) const {
	return rootSignatureShelf_->GetRootSignature(config);
}
