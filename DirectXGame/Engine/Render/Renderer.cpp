#include "Renderer.h"
#include <Render/Screen/IDisplay.h>

SHEngine::Renderer::Renderer(DrawData& drawData) : drawData_(drawData) {
}

void SHEngine::Renderer::SetGPUBuffer(GPUBuffer* gpuBuffer, ShaderType shaderType, BufferType bufferType) {
	assert(shaderType != ShaderType::COMPUTE_SHADER);
	gpuBuffer->GetBufferType();
	assert((gpuBuffer->GetBufferType() & bufferType) != 0);	//gpuBufferがbufferTypeのどれかには当てはまっているか
	gpuBuffers_[bufferType][shaderType].push_back(gpuBuffer);
}

void SHEngine::Renderer::Draw(CmdObj* cmdObj) {
	auto cmdList = cmdObj->GetCommandList();
	uint32_t cmdIndex = cmdObj->GetListIndex();
	auto display = cmdObj->GetRenderTarget();

	PSO::Config config;

	config.isSwapChain = display->GetRTVFormat() == DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	//PSOConfigを調整
	config.rootConfig.cbvNums = {
		int(gpuBuffers_[BufferType::CBV][ShaderType::VERTEX_SHADER].size()),
		int(gpuBuffers_[BufferType::CBV][ShaderType::PIXEL_SHADER].size())
	};
	config.rootConfig.srvNums = {
		int(gpuBuffers_[BufferType::SRV][ShaderType::VERTEX_SHADER].size()),
		int(gpuBuffers_[BufferType::SRV][ShaderType::PIXEL_SHADER].size())
	};
	//UAVはいつかやる
	config.rootConfig.useTexture = isUseTexture_;

	config.vs = vs_;
	config.ps = ps_;
	config.inputLayoutID = inputLayoutID_;
	config.blendID = blendID_;
	config.depthStencilID = depthStencilID_;
	config.rasterizerID = rasterizerID_;
	config.topology = topology_;
	config.isSwapChain = isSwapChain_;

	psoEditor_->SetPSO(cmdList, config);

	cmdList->IASetVertexBuffers(0, UINT(drawData_.vbv.size()), drawData_.vbv.data());
	cmdList->IASetIndexBuffer(&drawData_.ibv);

	int rootIndex = 0;
	for (const auto& [shaderType, cbvs] : gpuBuffers_[BufferType::CBV]) {
		for (const auto& cbv : cbvs) {
			cbv->TransitionBarrier(D3D12_RESOURCE_STATE_GENERIC_READ);
			cbv->Flush(cmdObj, cmdIndex);
			cmdList->SetGraphicsRootConstantBufferView(rootIndex++, cbv->GetGPUDescriptorHandle(BufferType::CBV, cmdIndex).ptr);
		}
	}
	for (const auto& [shaderType, srvs] : gpuBuffers_[BufferType::SRV]) {
		for (const auto& srv : srvs) {
			srv->TransitionBarrier(D3D12_RESOURCE_STATE_GENERIC_READ);
			srv->Flush(cmdObj, cmdIndex);
			cmdList->SetGraphicsRootDescriptorTable(rootIndex++, srv->GetGPUDescriptorHandle(BufferType::SRV, cmdIndex));
		}
	}
	for (const auto& [shaderType, uavs] : gpuBuffers_[BufferType::UAV]) {
		for (const auto& uav : uavs) {
			uav->TransitionBarrier(D3D12_RESOURCE_STATE_GENERIC_READ);
			uav->Flush(cmdObj, cmdIndex);
			cmdList->SetGraphicsRootDescriptorTable(rootIndex++, uav->GetGPUDescriptorHandle(BufferType::UAV, cmdIndex));
		}
	}

	if (isUseTexture_) {
		cmdList->SetGraphicsRootDescriptorTable(rootIndex++, textureStartHandle_);
	}

	cmdList->DrawIndexedInstanced(drawData_.indexNum, instanceNum_, 0, 0, 0);
}
