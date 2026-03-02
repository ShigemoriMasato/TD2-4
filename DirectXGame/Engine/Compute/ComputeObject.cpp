#include "ComputeObject.h"
#include <Utility/DirectUtilFuncs.h>

using namespace SHEngine;

DXDevice* ComputeObject::device_ = nullptr;
PSO::CSPSOManager* ComputeObject::psoManager_ = nullptr;

ComputeObject::ComputeObject(std::string debugName) {
	debugName_ = debugName;
	logger_->info("ComputeObject {} created", debugName_);
}

void ComputeObject::Initialize() {
	cbvAddress_.clear();
	srvHandle_.clear();
	uavHandle_.clear();
	cmdObj_ = nullptr;
	resources_.clear();
}

int ComputeObject::CreateCBV(size_t bufferSize) {
	Resource resource;
	resource.resource.Attach(Func::CreateBufferResource(device_->GetDevice(), bufferSize));
	cbvAddress_.push_back(resource.resource->GetGPUVirtualAddress());

	BufferData& bufferData = mappedBuffers_.emplace_back();
	resource.resource->Map(0, nullptr, &bufferData.mapped);
	bufferData.size = bufferSize;

	resources_.push_back(std::move(resource));

	return int(mappedBuffers_.size() - 1);
}

int ComputeObject::CreateSRV(size_t bufferSize, int num) {

	auto& srvHandle = srvHandle_.emplace_back(std::make_unique<SRVHandle>());
	srvHandle->UpdateHandle(device_->GetSRVManager());
	
	auto& resource = resources_.emplace_back();
	resource.resource.Attach(Func::CreateBufferResource(device_->GetDevice(), bufferSize * num));

	//ParticleDataのSRV作成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	srvDesc.Buffer.NumElements = num;
	srvDesc.Buffer.StructureByteStride = UINT(bufferSize);

	device_->GetDevice()->CreateShaderResourceView(resource.resource.Get(), &srvDesc, srvHandle->GetCPU());

	//Map
	BufferData& bufferData = mappedBuffers_.emplace_back();
	resource.resource->Map(0, nullptr, &bufferData.mapped);
	bufferData.size = bufferSize * num;

	return int(mappedBuffers_.size() - 1);
}

int ComputeObject::CreateUAV(size_t bufferSize, int num) {

	auto& uavHandle = uavHandle_.emplace_back(std::make_unique<SRVHandle>());
	uavHandle->UpdateHandle(device_->GetSRVManager());

	size_t totalSize = bufferSize * num;

	// UAVリソース (GPU専用)
	auto& uavReadback = uavReadbacks_.emplace_back();
	uavReadback.uavResource.Attach(Func::CreateUAVResource(device_->GetDevice(), totalSize));
	uavReadback.readbackResource.Attach(Func::CreateReadBackResource(device_->GetDevice(), totalSize));
	uavReadback.bufferSize = totalSize;

	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = num;
	uavDesc.Buffer.CounterOffsetInBytes = 0;
	uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
	uavDesc.Buffer.StructureByteStride = UINT(bufferSize);

	device_->GetDevice()->CreateUnorderedAccessView(uavReadback.uavResource.Get(), nullptr, &uavDesc, uavHandle->GetCPU());

	// resources_にも追加（Execute時に使用）
	Resource resource;
	resource.resource = uavReadback.uavResource;
	resources_.push_back(std::move(resource));

	return int(uavReadbacks_.size() - 1);
}

void ComputeObject::CopyBufferData(int index, const void* data, size_t dataSize) {
	if (index < 0 || index >= mappedBuffers_.size()) {
		logger_->error("Invalid buffer index: {}", index);
		return;
	}
	BufferData& bufferData = mappedBuffers_[index];
	if (dataSize > bufferData.size) {
		logger_->error("Data size exceeds buffer size: {} > {}", dataSize, bufferData.size);
		return;
	}
	std::memcpy(bufferData.mapped, data, dataSize);
}

void ComputeObject::GetUAVBuffer(int uavIndex, void* data, size_t dataSize) {
	if (!cmdObj_ || !cmdObj_->CanExecute()) {
		logger_->error("Now Executing or Never Execute. Please Check Used Command Object!");
		return;
	}

	if (uavIndex < 0 || uavIndex >= static_cast<int>(uavReadbacks_.size())) {
		logger_->error("Invalid UAV index: {}", uavIndex);
		return;
	}

	UAVReadback& uavReadback = uavReadbacks_[uavIndex];
	if (dataSize > uavReadback.bufferSize) {
		logger_->error("Data size exceeds buffer size: {} > {}", dataSize, uavReadback.bufferSize);
		return;
	}

	// Readbackバッファをマップしてデータを読み取る
	void* mappedData = nullptr;
	D3D12_RANGE readRange = { 0, dataSize };
	HRESULT hr = uavReadback.readbackResource->Map(0, &readRange, &mappedData);
	if (FAILED(hr)) {
		logger_->error("Failed to map readback buffer");
		return;
	}

	std::memcpy(data, mappedData, dataSize);

	D3D12_RANGE writeRange = { 0, 0 }; // 書き込みなし
	uavReadback.readbackResource->Unmap(0, &writeRange);

	cmdObj_ = nullptr;
}

void ComputeObject::Execute(CmdObj* cmdObj) {
	//PSOのセット
	psoManager_->SetPSO(cmdObj, int(cbvAddress_.size()), int(srvHandle_.size()), int(uavHandle_.size()), computeShaderName_);

	auto cmdList = cmdObj->GetCommandList();

	int rootIndex = 0;

	for(const auto& cbv : cbvAddress_) {
		cmdList->SetComputeRootConstantBufferView(rootIndex++, cbv);
	}
	for(const auto& srv : srvHandle_) {
		cmdList->SetComputeRootDescriptorTable(rootIndex++, srv->GetGPU());
	}
	for(const auto& uav : uavHandle_) {
		cmdList->SetComputeRootDescriptorTable(rootIndex++, uav->GetGPU());
	}

	cmdList->Dispatch((threadGroupSize_.x + 1023) / 1024, threadGroupSize_.y, threadGroupSize_.z);

	// UAVからReadbackバッファへコピー
	for (auto& uavReadback : uavReadbacks_) {
		// UAV -> COPY_SOURCE への遷移
		D3D12_RESOURCE_BARRIER barrier{};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Transition.pResource = uavReadback.uavResource.Get();
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		cmdList->ResourceBarrier(1, &barrier);

		// コピー実行
		cmdList->CopyResource(uavReadback.readbackResource.Get(), uavReadback.uavResource.Get());

		// COPY_SOURCE -> UAV への遷移（次回の実行に備えて）
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		cmdList->ResourceBarrier(1, &barrier);
	}

	cmdObj_ = cmdObj;
}
