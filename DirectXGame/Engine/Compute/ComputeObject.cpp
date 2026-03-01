#include "ComputeObject.h"
#include <Utility/DirectUtilFuncs.h>

using namespace SHEngine;

DXDevice* ComputeObject::device_ = nullptr;

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

	return int(mappedBuffers_.size() - 1);
}

int ComputeObject::CreateUAV(size_t bufferSize, int num) {

	auto& uavHandle = uavHandle_.emplace_back(std::make_unique<SRVHandle>());
	uavHandle->UpdateHandle(device_->GetSRVManager());

	auto& resource = resources_.emplace_back();
	resource.resource.Attach(Func::CreateBufferResource(device_->GetDevice(), bufferSize * num));

	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = num;
	uavDesc.Buffer.CounterOffsetInBytes = 0;
	uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
	uavDesc.Buffer.StructureByteStride = UINT(bufferSize);

	device_->GetDevice()->CreateUnorderedAccessView(resource.resource.Get(), nullptr, &uavDesc, uavHandle->GetCPU());

	//Map
	BufferData& bufferData = mappedBuffers_.emplace_back();
	resource.resource->Map(0, nullptr, &bufferData.mapped);

	return int(mappedBuffers_.size() - 1);
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

void ComputeObject::GetUAVBuffer(int index, void* data, size_t dataSize) {
	if (!cmdObj_ || !cmdObj_->CanExecute()) {
		logger_->error("Now Executing or Never Execute. Please Check Used Command Object!");
	}

	if (index < 0 || index >= mappedBuffers_.size()) {
		logger_->error("Invalid buffer index: {}", index);
		return;
	}
	BufferData& bufferData = mappedBuffers_[index];
	if (dataSize > bufferData.size) {
		logger_->error("Data size exceeds buffer size: {} > {}", dataSize, bufferData.size);
		return;
	}
	std::memcpy(data, bufferData.mapped, dataSize);
}

void ComputeObject::Execute(CmdObj* cmdObj) {

}
